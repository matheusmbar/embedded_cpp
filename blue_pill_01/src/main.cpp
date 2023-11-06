#include <FreeRTOS.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <task.h>

#include "etl/array.h"
#include "etl/string.h"
#include "etl/vector.h"
#include "test_cpp.h"

extern "C" {
    #include "test_c.h"
}

Test_CPP test_cpp(1001);

constexpr auto led_pin = GPIO13;
constexpr auto led_port = GPIOC;

void rcc_gpio_setup(void) {
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(led_port, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, led_pin);
}

void etl_log_error(const etl::exception& e) {
  std::printf("ERROR %s at %s:%d\n", e.what(), e.file_name(), e.line_number());
}

void task_blink (void* pvParameters) {
    uint32_t count = 0;

    for(;;) {
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

int check_inits () {
    int check_c;
    int check_cpp;

    check_c = test_c();
    if (check_c) {
        std::printf("check_c failed [%d]\n", check_c);
        return check_c;
    }
    else {
        std::printf("check_c succeeded\n");
    }

    if (1001 != test_cpp.getValue()) {
        return 1001;
    }

    check_cpp = test_cpp.checkGlobals();
    if (check_cpp) {
        std::printf("check_cpp failed [%d]\n", check_cpp);
        return check_cpp;
    }
    else {
        std::printf("check_cpp succeeded\n");
    }

    // printf calls _write() provided at syscalls.c
    std::printf("end of checks \n");

    return 0;
}

int main (void) {
    int check_c;
    int check_cpp;

    rcc_gpio_setup();

    etl::error_handler::set_callback<etl_log_error>();

    if (auto ret = check_inits()) {
        return ret;
    }

    xTaskCreate(task_blink, "blink", configMINIMAL_STACK_SIZE, (void*) NULL, 1, NULL);
    xTaskCreate(task_array, "array", configMINIMAL_STACK_SIZE, (void*)NULL, 2, NULL);
    xTaskCreate(task_vector, "vector", configMINIMAL_STACK_SIZE, (void*)NULL, 3, NULL);

    vTaskStartScheduler();

    for (;;) {
        // should not get here
    }
    return 0;
}
