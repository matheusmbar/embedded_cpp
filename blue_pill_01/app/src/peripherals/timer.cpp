#include "peripherals/timer.hpp"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

void timer2_setup(void) {
  /* Enable TIM2 clock. */
  rcc_periph_clock_enable(RCC_TIM2);
  /* Enable TIM2 interrupt. */
  nvic_enable_irq(NVIC_TIM2_IRQ);
  /* Reset TIM2 peripheral to defaults. */
  rcc_periph_reset_pulse(RST_TIM2);
  /* Set the prescaler to have the timer run at 10kHz */
  timer_set_prescaler(TIM2, (rcc_apb1_frequency / 10000));
  /* Disable preload, run continuously up to max value */
  timer_disable_preload(TIM2);
  timer_continuous_mode(TIM2);
  timer_set_period(TIM2, 65535);
  /* Counter enable. */
  timer_enable_counter(TIM2);
  /* Enable Channel 1 compare interrupt to increment 32bits count */
  timer_enable_irq(TIM2, TIM_DIER_CC1IE);
}
