#include <FreeRTOS.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
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

xSemaphoreHandle uart_semaphore;

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

  /* Finally enable the USART. */
  usart_enable(USART1);
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

void task_uart_rx(void* pvParameters) {
  auto cli = static_cast<EmbeddedCli*>(pvParameters);
  for (;;) {
    uint16_t data;
    // TODO(matheusmbar): replace this blocking call with receive from ISR
    data = usart_recv_blocking(USART1);
    embeddedCliReceiveChar(cli, static_cast<char>(data));
    xSemaphoreGive(uart_semaphore);
  }
}

void task_cli(void* pvParameters) {
  auto cli = static_cast<EmbeddedCli*>(pvParameters);
  for (;;) {
    embeddedCliProcess(cli);
    xSemaphoreTake(uart_semaphore, portMAX_DELAY);
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

  auto config = embeddedCliDefaultConfig();
  config->historyBufferSize = 32;
  config->enableAutoComplete = false;

  EmbeddedCli* cli = embeddedCliNew(config);
  cli->writeChar = writeChar;
  embeddedCliAddBinding(
      cli, {"led", "Control LED", true, static_cast<void*>(led_commands_queue), CliLed});

  uart_semaphore = xSemaphoreCreateBinary();

  xTaskCreate(task_blink, "blink", configMINIMAL_STACK_SIZE, static_cast<void*>(led_commands_queue),
              1, nullptr);
  xTaskCreate(task_uart_rx, "uart_rx", configMINIMAL_STACK_SIZE, static_cast<void*>(cli), 1,
              nullptr);
  xTaskCreate(task_cli, "task_cli", configMINIMAL_STACK_SIZE, static_cast<void*>(cli), 2, nullptr);

  vTaskStartScheduler();

  for (;;) {
    // should not get here
  }
  return 0;
}
