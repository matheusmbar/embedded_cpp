#include "peripherals/clock.hpp"

#include <libopencm3/stm32/rcc.h>

void clock_setup(void) {
  /* Select 72 MHz clock*/
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
}
