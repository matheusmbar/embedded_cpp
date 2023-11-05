#include <FreeRTOS.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <task.h>

constexpr auto led_pin = GPIO13;
constexpr auto led_port = GPIOC;

void rcc_gpio_setup(void) {
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
  rcc_periph_clock_enable(RCC_GPIOC);
  gpio_set_mode(led_port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL,
                led_pin);
}

void task_blink(void* pvParameters) {
  uint32_t count = 0;

  for (;;) {
    gpio_toggle(GPIOC, GPIO13);
    ++count;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

int main(void) {
  rcc_gpio_setup();

  xTaskCreate(task_blink, "blink", configMINIMAL_STACK_SIZE, (void*)NULL, 1,
              NULL);

  vTaskStartScheduler();

  for (;;) {
    // should not get here
  }
  return 0;
}
