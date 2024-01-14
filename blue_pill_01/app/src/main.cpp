#include <FreeRTOS.h>
#include <etl/algorithm.h>
#include <etl/string.h>
#include <etl/string_stream.h>
#include <etl/to_arithmetic.h>
#include <etl/to_string.h>
#include <etl/vector.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/usart.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#define EMBEDDED_CLI_IMPL
#include <embedded_cli.h>

#include "devices/gpio/gpio_opencm3.hpp"
#include "devices/lcd/ssd1306.hpp"
#include "led/led_gpio.hpp"
#include "led/led_interface.hpp"
#include "peripherals/clock.hpp"
#include "peripherals/i2c.hpp"
#include "peripherals/timer.hpp"
#include "peripherals/usart.hpp"
#include "sys/checks/test_cpp.hpp"

extern "C" {

int local_putchar(char ptr) {
  usart_send_blocking(USART1, ptr);
  return 0;
}
}

void writeChar(EmbeddedCli* /* embeddedCli */, char c) {
  local_putchar(c);
}

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
  timer2_setup();
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

namespace Snake {
constexpr auto kStep = 6;
constexpr auto kMinX = 6;
constexpr auto kMaxX = 120;
constexpr auto kMinY = 6;
constexpr auto kMaxY = 60;
constexpr auto kPosX = 1 + (kMaxX - kMinX) / kStep;
constexpr auto kPosY = 1 + (kMaxY - kMinY) / kStep;
constexpr auto kTotalPos = kPosX * kPosY;

static_assert((kMaxX - kMinX) % kStep == 0);
static_assert((kMaxY - kMinY) % kStep == 0);

enum class Action { kUp, kDown, kLeft, kRight, kNone };

};  // namespace Snake

etl::array<uint8_t, 2> UpdateHead(const etl::array<uint8_t, 2>& current_head,
                                  Snake::Action action) {
  static auto last_action = Snake::Action::kLeft;

  int16_t new_x = current_head[0];
  int16_t new_y = current_head[1];

  switch (action) {
    case Snake::Action::kUp:
      if (last_action == Snake::Action::kDown) {
        action = Snake::Action::kDown;
      }
      break;
    case Snake::Action::kDown:
      if (last_action == Snake::Action::kUp) {
        action = Snake::Action::kUp;
      }
      break;
    case Snake::Action::kLeft:
      if (last_action == Snake::Action::kRight) {
        action = Snake::Action::kRight;
      }
      break;
    case Snake::Action::kRight:
      if (last_action == Snake::Action::kLeft) {
        action = Snake::Action::kLeft;
      }
      break;
    case Snake::Action::kNone:
      action = last_action;
      break;
  }

  switch (action) {
    case Snake::Action::kUp:
      new_y -= Snake::kStep;
      break;
    case Snake::Action::kDown:
      new_y += Snake::kStep;
      break;
    case Snake::Action::kLeft:
      new_x -= Snake::kStep;
      break;
    case Snake::Action::kRight:
      new_x += Snake::kStep;
      break;
    default:
      break;
  }

  if (new_x < Snake::kMinX) {
    new_x = Snake::kMaxX;
  } else if (new_x > Snake::kMaxX) {
    new_x = Snake::kMinX;
  }

  if (new_y < Snake::kMinY) {
    new_y = Snake::kMaxY;
  } else if (new_y > Snake::kMaxY) {
    new_y = Snake::kMinY;
  }
  last_action = action;

  return etl::array<uint8_t, 2>({static_cast<uint8_t>(new_x), static_cast<uint8_t>(new_y)});
}

void task_lcd(void* /*pvParameters*/) {
  auto btn_up = std::make_shared<GpioOpencm3>(GpioFunction::kInputPullDown, GPIOA, GPIO4);
  auto btn_left = std::make_shared<GpioOpencm3>(GpioFunction::kInputPullDown, GPIOA, GPIO3);
  auto btn_down = std::make_shared<GpioOpencm3>(GpioFunction::kInputPullDown, GPIOA, GPIO2);
  auto btn_center = std::make_shared<GpioOpencm3>(GpioFunction::kInputPullDown, GPIOA, GPIO1);
  auto btn_right = std::make_shared<GpioOpencm3>(GpioFunction::kInputPullDown, GPIOA, GPIO0);

  SSD1306 lcd{I2C1, btn_center, nullptr, btn_right, btn_left, btn_up, btn_down};
  etl::string<15> msg{"Hello world"};

  etl::vector<std::shared_ptr<GpioOpencm3>, 6> buttons;
  buttons.push_back(btn_up);
  buttons.push_back(btn_left);
  buttons.push_back(btn_down);
  buttons.push_back(btn_center);
  buttons.push_back(btn_right);

  const etl::vector<etl::array<uint8_t, 2>, 4> fruits{
      {Snake::kMinX + Snake::kStep * 4, Snake::kMinY + Snake::kStep * 3},
      {Snake::kMinX + Snake::kStep * 6, Snake::kMinY + Snake::kStep * 6},
      {Snake::kMinX + Snake::kStep * 1, Snake::kMinY + Snake::kStep * 1},
      {Snake::kMinX + Snake::kStep * 13, Snake::kMinY + Snake::kStep * 5}};

  int max_points = 0;

  for (;;) {
    etl::vector<etl::array<uint8_t, 2>, Snake::kTotalPos> snake;
    for (uint8_t i = 0; i < 5; i++) {
      uint8_t next_x = static_cast<uint8_t>(Snake::kStep) * i + 60;
      snake.push_back(etl::array<uint8_t, 2>({next_x, 30}));
    }

    auto fruit = fruits.begin();
    auto head = snake.begin();
    int points = 0;

    UpdateHead(*snake.begin(), Snake::Action::kUp);
    UpdateHead(*snake.begin(), Snake::Action::kLeft);
    lcd.ClearDisplay();

    for (;;) {
      lcd.ClearBuffer();

      etl::to_string<size_t>(points, msg);
      lcd.DrawStr(105, 15, msg);

      for (const auto& section : snake) {
        lcd.DrawCircle(section[0], section[1], 2);
      }
      lcd.DrawCircle((*snake.begin())[0], (*snake.begin())[1], 1);

      lcd.DrawLine((*fruit)[0] - 2, (*fruit)[1] - 2, (*fruit)[0] + 2, (*fruit)[1] + 2);
      lcd.DrawLine((*fruit)[0] - 2, (*fruit)[1] + 2, (*fruit)[0] + 2, (*fruit)[1] - 2);

      bool eat_fruit = false;
      if (*head == *fruit) {
        eat_fruit = true;
        ++points;
        ++fruit;
        if (fruit == fruits.end()) {
          fruit = fruits.begin();
        }
      }

      lcd.Refresh();

      Snake::Action btn_action{Snake::Action::kNone};
      for (auto i = 0; i < 10; i++) {
        if (btn_up->Get() == GpioState::kHigh) {
          btn_action = Snake::Action::kUp;
        } else if (btn_down->Get() == GpioState::kHigh) {
          btn_action = Snake::Action::kDown;
        } else if (btn_left->Get() == GpioState::kHigh) {
          btn_action = Snake::Action::kLeft;
        } else if (btn_right->Get() == GpioState::kHigh) {
          btn_action = Snake::Action::kRight;
        }
        int wait_ms = 20 - points;
        wait_ms = wait_ms < 1 ? 1 : wait_ms;
        vTaskDelay(pdMS_TO_TICKS(wait_ms));
      }

      if (!eat_fruit) {
        snake.pop_back();
      }
      auto new_head = UpdateHead(*snake.begin(), btn_action);
      if (etl::find(snake.begin(), snake.end(), new_head) != snake.end()) {
        break;
      }

      snake.insert(snake.begin(), new_head);
      head = snake.begin();
    }

    while (btn_center->Get() != GpioState::kHigh) {
      vTaskDelay(pdMS_TO_TICKS(50));
    }

    lcd.ClearBuffer();
    msg = "GAME OVER";
    lcd.DrawStr(30, 30, msg);
    etl::to_string<int>(points, msg);
    lcd.DrawStr(60, 45, msg);
    if (points > max_points) {
      max_points = points;
      msg = "New record!";
    } else {
      msg = "  Record: ";
      etl::to_string<int>(max_points, msg, true);
    }
    lcd.DrawStr(35, 60, msg);

    lcd.Refresh();

    while (btn_center->Get() != GpioState::kLow) {
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    while (btn_center->Get() != GpioState::kHigh) {
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
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
