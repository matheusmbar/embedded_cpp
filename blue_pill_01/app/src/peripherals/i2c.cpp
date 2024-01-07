#include "peripherals/i2c.hpp"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>

void i2c_setup(void) {
  /* Enable clocks for I2C2 and AFIO. */
  rcc_periph_clock_enable(RCC_I2C1);
  rcc_periph_clock_enable(RCC_AFIO);
  rcc_periph_clock_enable(RCC_GPIOB);

  /* Set alternate functions for the SCL (PB6) and SDA (PB7) pins of I2C2. */
  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
                GPIO_I2C1_SCL | GPIO_I2C1_SDA);

  /* Disable the I2C before changing any configuration. */
  i2c_peripheral_disable(I2C1);

  /* APB1 is running at 36MHz. */
  i2c_set_clock_frequency(I2C1, 72);

  /* 400KHz - I2C Fast Mode */
  i2c_set_fast_mode(I2C1);

  i2c_set_speed(I2C1, i2c_speed_sm_100k, 72);

  /*
   * fclock for I2C is 36MHz APB2 -> cycle time 28ns, low time at 400kHz
   * incl trise -> Thigh = 1600ns; CCR = tlow/tcycle = 0x1C,9;
   * Datasheet suggests 0x1e.
   */
  i2c_set_ccr(I2C1, 0x1e);

  /*
   * fclock for I2C is 36MHz -> cycle time 28ns, rise time for
   * 400kHz => 300ns and 100kHz => 1000ns; 300ns/28ns = 10;
   * Incremented by 1 -> 11.
   */
  i2c_set_trise(I2C1, 0x0b);

  /* If everything is configured -> enable the peripheral. */
  i2c_peripheral_enable(I2C1);
}
