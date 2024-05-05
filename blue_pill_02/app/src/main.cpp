#include <FreeRTOS.h>
#include <etl/string.h>
#include <etl/string_stream.h>
#include <libopencm3/stm32/gpio.h>
#include <projdefs.h>
#include <task.h>

#include <memory>

#include "FreeRTOSConfig.h"
#include "factory.hpp"
#include "peripherals/clock.hpp"
#include "peripherals/usart.hpp"

void etl_log_error(const etl::exception& e) {
  etl::string<80> error_msg;
  etl::string_stream stream(error_msg);
  stream << e.file_name() << ":" << e.line_number() << " ERROR " << e.what() << "\r\n";

  usart1_send(error_msg);
}

void DelayMs(uint32_t wait_ms) {
  vTaskDelay(pdMS_TO_TICKS(wait_ms));
}

void BlinkTask(void* arg) {
  auto led_ptr = reinterpret_cast<std::shared_ptr<LedInterface>*>(arg);
  auto led = std::shared_ptr<LedInterface>(*led_ptr);
  for (;;) {
    led->Toggle();
    DelayMs(500);
  }
}

int main() {
  clock_setup();
  usart1_setup();

  usart1_send(etl::string<15>("\r\nBOOTING\r\n"));
  usart1_send(etl::string<30>("Compiled at " __DATE__ " " __TIME__ "\r\n"));

  etl::error_handler::set_callback<etl_log_error>();

  auto led = MakeLed(MakeGpio(GpioFunction::kOutput, GPIOC, GPIO13));
  xTaskCreate(BlinkTask, "blink", configMINIMAL_STACK_SIZE, reinterpret_cast<void*>(&led), 2,
              nullptr);

  vTaskStartScheduler();

  for (;;) {
    // should not get here
  }
  return 0;
}
