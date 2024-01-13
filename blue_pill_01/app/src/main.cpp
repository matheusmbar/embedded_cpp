#include <FreeRTOS.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#include "devices/gpio/gpio_opencm3.hpp"
#include "devices/lcd/ssd1306.hpp"
#include "etl/string.h"
#include "etl/string_stream.h"
#include "etl/to_arithmetic.h"
#include "etl/to_string.h"
#include "etl/vector.h"
#include "led/led_gpio.hpp"
#include "led/led_interface.hpp"
#include "mymath.hpp"
#include "peripherals/clock.hpp"
#include "peripherals/i2c.hpp"
#include "peripherals/usart.hpp"
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

void etl_log_error(const etl::exception& e) {
  etl::string<80> error_msg;
  etl::string_stream stream(error_msg);
  stream << e.file_name() << ":" << e.line_number() << " ERROR " << e.what();

  if (Globals::cli_) {
    embeddedCliPrint(Globals::cli_, error_msg.c_str());
  } else {
    std::printf("%s\r\n", error_msg.c_str());
  }
}

void task_blink(void* pvParameters) {
  auto led_commands_queue = static_cast<QueueHandle_t>(pvParameters);
  LedGpio led(std::make_shared<GpioOpencm3>(GpioFunction::kOutput, led_port, led_pin), true);
  LedCommand cmd{LedMode::kOff, 0};
  auto ticks = portMAX_DELAY;
  for (;;) {
    ticks = portMAX_DELAY;
    switch (cmd.mode) {
      case LedMode::kOff:
        led.SetOff();
        break;
      case LedMode::kOn:
        led.SetOn();
        break;
      case LedMode::kToggle:
        led.Toggle();
        break;
      case LedMode::kBlink:
        led.Toggle();
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

void task_lcd(void* /*pvParameters*/) {
  auto btn_up = std::make_shared<GpioOpencm3>(GpioFunction::kInputPullDown, GPIOA, GPIO4);
  auto btn_left = std::make_shared<GpioOpencm3>(GpioFunction::kInputPullDown, GPIOA, GPIO3);
  auto btn_down = std::make_shared<GpioOpencm3>(GpioFunction::kInputPullDown, GPIOA, GPIO2);
  auto btn_center = std::make_shared<GpioOpencm3>(GpioFunction::kInputPullDown, GPIOA, GPIO1);
  auto btn_right = std::make_shared<GpioOpencm3>(GpioFunction::kInputPullDown, GPIOA, GPIO0);

  SSD1306 lcd{I2C1, btn_center, nullptr, btn_right, btn_left, btn_up, btn_down};
  uint8_t count = 0;
  etl::string<15> msg{"Hello world"};

  etl::vector<std::shared_ptr<GpioOpencm3>, 6> buttons;
  buttons.push_back(btn_up);
  buttons.push_back(btn_left);
  buttons.push_back(btn_down);
  buttons.push_back(btn_center);
  buttons.push_back(btn_right);

  for (;;) {
    switch (count) {
      case 1: {
        lcd.ClearDisplay();
        lcd.DrawStr(0, 25, msg);

        uint8_t pos = 50;
        for (const auto& btn : buttons) {
          if (btn->Get() == GpioState::kHigh) {
            lcd.DrawCircle(pos, 55, 2);
          } else {
            lcd.DrawCircle(pos, 60, 2);
          }
          pos += 4;
        }
      } break;
      case 2:
        lcd.DrawCircle(112, 45, 13);
        lcd.DrawBox(0, 40, 10, 10);
        break;
      case 3:
        lcd.DrawLine(0, 27, 127, 63);
        lcd.DrawLine(0, 63, 127, 27);
        break;
      case 4:
        break;
      default:
        count = 0;
        break;
    }
    ++count;

    lcd.Refresh();
    vTaskDelay(pdMS_TO_TICKS(500));
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
  constexpr auto commands = "[on, off, toggle, blink <period_ms>]";
  const char* arg_1 = embeddedCliGetToken(args, 1);
  if (arg_1 == nullptr) {
    embeddedCliPrint(cli, "\tMissing command");
    embeddedCliPrint(cli, commands);
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
    embeddedCliPrint(cli, commands);
    return;
  }
  xQueueSend(led_commands_queue, &led_cmd, 0);
}

void CliStats(EmbeddedCli* cli, char* args, void* context) {
  etl::array<char, 200> buffer;
  // this function uses a lot of stack memory (~500 bytes for 3 tasks)
  // and around 2KB in binary flash size as well
  // TODO(matheusmbar) try optimizing this by removing stats pretty print
  vTaskGetRunTimeStats(buffer.data());
  embeddedCliPrint(cli, buffer.data());
}

int main(void) {
  static_assert(__cplusplus == 201703);

  clock_setup();
  usart1_setup();
  i2c_setup();

  std::printf("\r\nBOOTING\r\n");
  std::printf("Compiled at %s %s\r\n", __DATE__, __TIME__);
  std::printf("PERIPHERALS INITIALIZED\r\n");

  etl::error_handler::set_callback<etl_log_error>();

  ETL_ASSERT(check_inits() == 0, etl::exception("check_inits failed", __FILE__, __LINE__))

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

  xTaskCreate(task_lcd, "task_lcd", 4 * configMINIMAL_STACK_SIZE, nullptr, 2, nullptr);

  vTaskStartScheduler();

  for (;;) {
    // should not get here
  }
  return 0;
}
