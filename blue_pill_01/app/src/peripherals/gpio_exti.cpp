#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

extern "C" {
void exti0_isr() {
  exti_reset_request(EXTI0);
}
void exti1_isr() {
  exti_reset_request(EXTI1);
}
void exti2_isr() {
  exti_reset_request(EXTI2);
}
void exti3_isr() {
  exti_reset_request(EXTI3);
}
void exti4_isr() {
  exti_reset_request(EXTI4);
}
}

void setup_gpio_exti() {
  /* Enable AFIO clock. */
  rcc_periph_clock_enable(RCC_AFIO);

  /* Enable EXTI0 interrupt. */
  nvic_enable_irq(NVIC_EXTI0_IRQ);
  nvic_enable_irq(NVIC_EXTI1_IRQ);
  nvic_enable_irq(NVIC_EXTI2_IRQ);
  nvic_enable_irq(NVIC_EXTI3_IRQ);
  nvic_enable_irq(NVIC_EXTI4_IRQ);

  /* Configure the EXTI subsystem. */
  exti_select_source(EXTI0 | EXTI1 | EXTI2 | EXTI3 | EXTI4, GPIOA);
  exti_set_trigger(EXTI0 | EXTI1 | EXTI2 | EXTI3 | EXTI4, EXTI_TRIGGER_RISING);
  exti_enable_request(EXTI0 | EXTI1 | EXTI2 | EXTI3 | EXTI4);
}
