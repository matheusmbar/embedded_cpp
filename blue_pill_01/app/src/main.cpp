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

#include "devices/button/button_gpio.hpp"
#include "devices/gpio/gpio_opencm3.hpp"
#include "devices/keypad/keypad_button.hpp"
#include "devices/lcd/ssd1306.hpp"
#include "factory.hpp"
#include "peripherals/clock.hpp"
#include "peripherals/i2c.hpp"
#include "peripherals/timer.hpp"
#include "peripherals/usart.hpp"
#include "snake.hpp"
#include "snake_ui.hpp"
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

namespace Globals {
uint32_t timer32bits = 0;
xSemaphoreHandle uart_semaphore;
EmbeddedCli* cli_{nullptr};
}  // namespace Globals

extern "C" {
void usart1_isr(void) {
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

void tim2_isr(void) {
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

void task_cli(void* pvParameters) {
  auto cli = static_cast<EmbeddedCli*>(pvParameters);
  for (;;) {
    embeddedCliProcess(cli);
    xSemaphoreTake(Globals::uart_semaphore, portMAX_DELAY);
  }
}

void task_lcd(void* /*pvParameters*/) {
  auto btn_up = MakeButton(MakeGpio(GpioFunction::kInputPullDown, GPIOA, GPIO4), GpioState::kHigh);
  auto btn_left =
      MakeButton(MakeGpio(GpioFunction::kInputPullDown, GPIOA, GPIO3), GpioState::kHigh);
  auto btn_down =
      MakeButton(MakeGpio(GpioFunction::kInputPullDown, GPIOA, GPIO2), GpioState::kHigh);
  auto btn_center =
      MakeButton(MakeGpio(GpioFunction::kInputPullDown, GPIOA, GPIO1), GpioState::kHigh);
  auto btn_right =
      MakeButton(MakeGpio(GpioFunction::kInputPullDown, GPIOA, GPIO0), GpioState::kHigh);

  auto keypad = MakeKeypad(btn_up, btn_left, btn_down, btn_right, btn_center);

  SSD1306 lcd{I2C1, keypad};
  lcd.SetFont(SSD1306::Font::k5x7_Tn);
  etl::string<15> msg{"Hello world"};

  const etl::vector<Position, 4> fruits{Position(4, 3), Position(6, 6), Position(1, 1),
                                        Position(13, 5)};

  int max_points = 0;

  for (;;) {
    Snake snake;
    SnakeUi ui(snake, lcd, keypad);

    auto fruit = fruits.begin();
    int points = 0;

    snake.Reset();
    ui.Reset();

    while (!snake.ColisionDetected()) {
      ui.DrawGame(*fruit, points);

      if (snake.ProcessFruit(*fruit)) {
        ++fruit;
        fruit = fruit == fruits.end() ? fruits.begin() : fruit;
      }
      points = snake.GetPoints();

      auto btn_action{Snake::Action::kNone};

      for (auto i = 0; i < 10; i++) {
        if (auto action = ui.GetAction(); action != Snake::Action::kNone) {
          btn_action = action;
        }
        int wait_ms = 20 - points;
        wait_ms = wait_ms < 1 ? 1 : wait_ms;
        vTaskDelay(pdMS_TO_TICKS(wait_ms));
      }

      snake.ProcessAction(btn_action);
    }

    while (!btn_center->IsPressed()) {
      vTaskDelay(pdMS_TO_TICKS(50));
    }

    if (points > max_points) {
      max_points = points;
    }
    ui.DrawGameOver(points, max_points);

    while (btn_center->IsPressed()) {
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    while (!btn_center->IsPressed()) {
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    while (btn_center->IsPressed()) {
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }
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

  Globals::uart_semaphore = xSemaphoreCreateBinary();

  auto config = embeddedCliDefaultConfig();
  config->historyBufferSize = 32;
  config->enableAutoComplete = false;

  Globals::cli_ = embeddedCliNew(config);
  Globals::cli_->writeChar = writeChar;

  embeddedCliAddBinding(Globals::cli_, {"stats", "Get FreeRTOS stats", true, nullptr, CliStats});

  xTaskCreate(task_cli, "task_cli", 3 * configMINIMAL_STACK_SIZE, static_cast<void*>(Globals::cli_),
              2, nullptr);

  xTaskCreate(task_lcd, "task_lcd", 4 * configMINIMAL_STACK_SIZE, nullptr, 2, nullptr);

  vTaskStartScheduler();

  for (;;) {
    // should not get here
  }
  return 0;
}
