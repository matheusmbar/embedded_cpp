#include <FreeRTOS.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <task.h>

#include "etl/array.h"
#include "etl/string.h"
#include "etl/vector.h"
#include "mymath.h"
#include "test_cpp.h"

extern "C" {
#include "test_c.h"

int local_putchar(char ptr) {
  usart_send_blocking(USART1, ptr);
  return 0;
}
}

Test_CPP test_cpp(1001);

constexpr auto led_pin = GPIO13;
constexpr auto led_port = GPIOC;

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

  /* Setup UART parameters. */
  usart_set_baudrate(USART1, 115200);
  usart_set_databits(USART1, 8);
  usart_set_stopbits(USART1, USART_STOPBITS_1);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
  usart_set_mode(USART1, USART_MODE_TX);

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
  uint32_t count = 0;

  for (;;) {
    gpio_toggle(GPIOC, GPIO13);
    ++count;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void task_array(void* pvParameters) {
  const etl::string<11> msg{"Task array"};
  std::printf("%s\n", msg.c_str());
  etl::array<uint8_t, 10> arr;

  for (uint8_t i = 0; i < arr.max_size(); i++) {
    arr.insert_at(i, i);
  }
  vTaskDelete(NULL);
}

void task_vector(void* pvParameters) {
  const etl::string<11> msg{"Task vector"};
  std::printf("%s\n", msg.c_str());
  etl::vector<uint8_t, 10> vec;

  for (uint8_t i = 0; i < vec.max_size(); i++) {
    vec.push_back(i);
  }
  vTaskDelete(NULL);
}

int check_inits() {
  int check_c;
  int check_cpp;

  check_c = test_c();
  if (check_c) {
    std::printf("check_c failed [%d]\n", check_c);
    return check_c;
  } else {
    std::printf("check_c succeeded\n");
  }

  if (1001 != test_cpp.getValue()) {
    return 1001;
  }

  check_cpp = test_cpp.checkGlobals();
  if (check_cpp) {
    std::printf("check_cpp failed [%d]\n", check_cpp);
    return check_cpp;
  } else {
    std::printf("check_cpp succeeded\n");
  }

  // printf calls _write() provided at syscalls.c
  std::printf("end of checks \n");

  return 0;
}

int main(void) {
  static_assert(__cplusplus == 201703);

  int check_c;
  int check_cpp;

  clock_setup();
  gpio_setup();
  usart_setup();

  std::printf("\nBOOTING\n");
  std::printf("PERIPHERALS INITIALIZED\n\n");

  etl::error_handler::set_callback<etl_log_error>();

  if (auto ret = check_inits()) {
    return ret;
  }

  int v = MyMath::Sum(1, 2);

  xTaskCreate(task_blink, "blink", configMINIMAL_STACK_SIZE, nullptr, 1, nullptr);
  xTaskCreate(task_array, "array", configMINIMAL_STACK_SIZE, nullptr, 2, nullptr);
  xTaskCreate(task_vector, "vector", configMINIMAL_STACK_SIZE, nullptr, 3, nullptr);

  vTaskStartScheduler();

  for (;;) {
    // should not get here
  }
  return 0;
}
