#include <FreeRTOS.h>
#include <task.h>

#include <bootutil/bootutil_public.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <tinycrypt/sha256.h>

#include "test_cpp.h"

#include <string>

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

void task_blink (void* pvParameters) {
    uint32_t count = 0;

    for(;;) {
        gpio_toggle(GPIOC, GPIO13);
        ++count;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_confirm_boot (void* pvParameters) {
    // Test Bootutil compilation
    boot_set_confirmed_multi(0);

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

    if (auto ret = check_inits()) {
        return ret;
    }

    // Test Tinycrypt compilation
    TCSha256State_t sha256_state;
    tc_sha256_init(sha256_state);

    xTaskCreate(task_confirm_boot, "confirm_boot", configMINIMAL_STACK_SIZE, (void*) NULL, 2, NULL);

    xTaskCreate(task_blink, "blink", configMINIMAL_STACK_SIZE, (void*) NULL, 1, NULL);

    vTaskStartScheduler();

    for (;;) {
        // should not get here
    }
    return 0;
}
