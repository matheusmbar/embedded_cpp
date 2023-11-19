#include <FreeRTOS.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#include "etl/string.h"
#include "etl/to_arithmetic.h"
#include "etl/to_string.h"
#include "led.hpp"
#include "mymath.hpp"
#include "test_cpp.hpp"

#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"

extern "C" {
#include "test_c.h"

int local_putchar(char ptr) {
  usart_send_blocking(USART1, ptr);
  return 0;
}
}

void writeChar(EmbeddedCli* /* embeddedCli */, char c) {
  local_putchar(c);
}

Test_CPP test_cpp(1001);

constexpr auto led_pin = GPIO13;
constexpr auto led_port = GPIOC;

namespace Globals {
uint32_t timer32bits = 0;
xSemaphoreHandle uart_semaphore;
EmbeddedCli* cli_{nullptr};
}  // namespace Globals

static void clock_setup(void) {
  /* Select 72 MHz clock*/
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

  /* Enable clock for GPIO port C (for LED pin) */
  rcc_periph_clock_enable(RCC_GPIOC);

  /* Enable clocks for GPIO port A (for GPIO_USART1_TX) and USART1. */
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_AFIO);
  rcc_periph_clock_enable(RCC_USART1);
}

static void usart_setup(void) {
  /* Enable the USART1 interrupt. */
  nvic_enable_irq(NVIC_USART1_IRQ);

  /* Setup GPIO pin GPIO_USART1_RE_TX on GPIO port A for transmit. */
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
  /* Setup GPIO pin GPIO_USART1_RE_RX on GPIO port A for receive. */
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_RX);

  /* Setup UART parameters. */
  usart_set_baudrate(USART1, 115200);
  usart_set_databits(USART1, 8);
  usart_set_stopbits(USART1, USART_STOPBITS_1);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
  usart_set_mode(USART1, USART_MODE_TX_RX);

  /* Enable USART1 Receive interrupt. */
  USART_CR1(USART1) |= USART_CR1_RXNEIE;

  /* Finally enable the USART. */
  usart_enable(USART1);
}

extern "C" {
void USART1_IRQHandler(void) {
  /* Check if we were called because of RXNE. */
  if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) && ((USART_SR(USART1) & USART_SR_RXNE) != 0)) {
    uint16_t data = usart_recv(USART1);
    if (Globals::cli_) {
      embeddedCliReceiveChar(Globals::cli_, static_cast<char>(data));
      BaseType_t task_woken{pdFALSE};
      xSemaphoreGiveFromISR(Globals::uart_semaphore, &task_woken);
      if (task_woken) {
        portYIELD()
      }
    }
  }
}

void SetupStatsTimer(void) {
  /* Enable TIM2 clock. */
  rcc_periph_clock_enable(RCC_TIM2);
  /* Enable TIM2 interrupt. */
  nvic_enable_irq(NVIC_TIM2_IRQ);
  /* Reset TIM2 peripheral to defaults. */
  rcc_periph_reset_pulse(RST_TIM2);
  /* Set the prescaler to have the timer run at 10kHz */
  timer_set_prescaler(TIM2, (rcc_apb1_frequency / 10000));
  /* Disable preload, run continuously up to max value */
  timer_disable_preload(TIM2);
  timer_continuous_mode(TIM2);
  timer_set_period(TIM2, 65535);
  /* Counter enable. */
  timer_enable_counter(TIM2);
  /* Enable Channel 1 compare interrupt to increment 32bits count */
  timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}

uint32_t GetStatsTimerCount(void) {
  return Globals::timer32bits + timer_get_counter(TIM2);
}

void TIM2_IRQHandler(void) {
  if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {
    /* Clear compare interrupt flag. */
    timer_clear_flag(TIM2, TIM_SR_CC1IF);
    Globals::timer32bits += UINT16_MAX;
  }
}
}

static void gpio_setup(void) {
  /* Configure LED GPIO */
  gpio_set_mode(led_port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, led_pin);
}

void etl_log_error(const etl::exception& e) {
  std::printf("ERROR %s\n", e.what());
}

void task_blink(void* pvParameters) {
  auto led_commands_queue = static_cast<QueueHandle_t>(pvParameters);

  LedCommand cmd{LedMode::kOff, 0};
  auto ticks = portMAX_DELAY;
  for (;;) {
    switch (cmd.mode) {
      case LedMode::kOff:
        gpio_set(GPIOC, GPIO13);
        ticks = portMAX_DELAY;
        break;
      case LedMode::kOn:
        gpio_clear(GPIOC, GPIO13);
        ticks = portMAX_DELAY;
        break;
      case LedMode::kToggle:
        gpio_toggle(GPIOC, GPIO13);
        ticks = portMAX_DELAY;
        break;
      case LedMode::kBlink:
        gpio_toggle(GPIOC, GPIO13);
        ticks = pdMS_TO_TICKS(cmd.period_ms / 2);
        break;
    }
    xQueueReceive(led_commands_queue, &cmd, ticks);
  }
}

void task_cli(void* pvParameters) {
  auto cli = static_cast<EmbeddedCli*>(pvParameters);
  for (;;) {
    embeddedCliProcess(cli);
    xSemaphoreTake(Globals::uart_semaphore, portMAX_DELAY);
  }
}

int check_inits() {
  if (auto check_c = test_c(); check_c) {
    std::printf("check_c failed [%d]\n", check_c);
    return check_c;
  }

  if (1001 != test_cpp.getValue()) {
    return 1001;
  }

  if (auto check_cpp = test_cpp.checkGlobals(); check_cpp) {
    std::printf("check_cpp failed [%d]\n", check_cpp);
    return check_cpp;
  }
  return 0;
}

void CliLed(EmbeddedCli* cli, char* args, void* context) {
  auto led_commands_queue = static_cast<QueueHandle_t>(context);
  LedCommand led_cmd;
  embeddedCliPrint(cli, "LED");
  const char* arg_1 = embeddedCliGetToken(args, 1);
  if (arg_1 == nullptr) {
    embeddedCliPrint(cli, "\tMissing command");
    return;
  }

  const etl::string<7> arg_cmd{arg_1};
  if (arg_cmd.compare("on") == 0) {
    embeddedCliPrint(cli, "\ton");
    led_cmd.mode = LedMode::kOn;
  } else if (arg_cmd.compare("off") == 0) {
    embeddedCliPrint(cli, "\toff");
    led_cmd.mode = LedMode::kOff;
  } else if (arg_cmd.compare("toggle") == 0) {
    embeddedCliPrint(cli, "\ttoggle");
    led_cmd.mode = LedMode::kToggle;
  } else if (arg_cmd.compare("blink") == 0) {
    embeddedCliPrint(cli, "\tblink");
    const char* arg_period = embeddedCliGetToken(args, 2);
    if (arg_period == nullptr) {
      embeddedCliPrint(cli, "\tMissing blink period");
      return;
    }
    etl::string<6> period{arg_period};
    if (auto result = etl::to_arithmetic<uint32_t>(period); result) {
      led_cmd.mode = LedMode::kBlink;
      led_cmd.period_ms = result.value();
    } else {
      embeddedCliPrint(cli, "\tInvalid blink period");
      return;
    }
  } else {
    embeddedCliPrint(cli, "\tInvalid command");
    return;
  }
  xQueueSend(led_commands_queue, &led_cmd, 0);
}

void CliStats(EmbeddedCli* cli, char* args, void* context) {
  etl::array<char, 200> buffer;
  // this function uses a lot of stack memory (~500 bytes for 3 tasks)
  vTaskGetRunTimeStats(buffer.data());
  embeddedCliPrint(cli, buffer.data());
}

int main(void) {
  static_assert(__cplusplus == 201703);

  clock_setup();
  gpio_setup();
  usart_setup();

  std::printf("\r\nBOOTING\r\n");
  std::printf("PERIPHERALS INITIALIZED\r\n");

  etl::error_handler::set_callback<etl_log_error>();

  assert(check_inits() == 0);

  QueueHandle_t led_commands_queue = xQueueCreate(1, sizeof(LedCommand));
  Globals::uart_semaphore = xSemaphoreCreateBinary();

  auto config = embeddedCliDefaultConfig();
  config->historyBufferSize = 32;
  config->enableAutoComplete = false;

  Globals::cli_ = embeddedCliNew(config);
  Globals::cli_->writeChar = writeChar;
  embeddedCliAddBinding(
      Globals::cli_, {"led", "Control LED", true, static_cast<void*>(led_commands_queue), CliLed});
  embeddedCliAddBinding(Globals::cli_, {"stats", "Get FreeRTOS stats", true, nullptr, CliStats});

  xTaskCreate(task_blink, "blink", configMINIMAL_STACK_SIZE, static_cast<void*>(led_commands_queue),
              1, nullptr);
  xTaskCreate(task_cli, "task_cli", 3 * configMINIMAL_STACK_SIZE, static_cast<void*>(Globals::cli_),
              2, nullptr);

  vTaskStartScheduler();

  for (;;) {
    // should not get here
  }
  return 0;
}
