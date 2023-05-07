
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

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

int main (void) {
    int check_c;
    int check_cpp;

    rcc_gpio_setup();

    while(1) {
        gpio_toggle(led_port, led_pin);

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
        std::printf("end of loop \n");
    }
    return 0;
}
