#include "devices/gpio/gpio_opencm3.hpp"

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

GpioOpencm3::GpioOpencm3(GpioFunction function, uint32_t port, uint16_t pin)
    : function_(function), port_(port), pin_(pin) {
  switch (port_) {
    case GPIOA:
      rcc_periph_clock_enable(RCC_GPIOA);
      break;
    case GPIOB:
      rcc_periph_clock_enable(RCC_GPIOB);
      break;
    case GPIOC:
      rcc_periph_clock_enable(RCC_GPIOC);
      break;
    case GPIOD:
      rcc_periph_clock_enable(RCC_GPIOD);
      break;
    case GPIOE:
      rcc_periph_clock_enable(RCC_GPIOE);
      break;
    case GPIOF:
      rcc_periph_clock_enable(RCC_GPIOF);
      break;
    case GPIOG:
      rcc_periph_clock_enable(RCC_GPIOG);
      break;
    default:
      break;
  }

  switch (function_) {
    case GpioFunction::kInput:
      break;
    case GpioFunction::kOutput:
      gpio_set_mode(port_, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, pin_);
      break;
  }
}

GpioState GpioOpencm3::Get() const {
  if (gpio_get(port_, pin_)) {
    return GpioState::kHigh;
  } else {
    return GpioState::kLow;
  }
}

void GpioOpencm3::Set(GpioState state) {
  switch (state) {
    case GpioState::kHigh:
      SetHigh();
      break;
    case GpioState::kLow:
      SetLow();
      break;
    case GpioState::kUnknown:
      break;
  }
}

void GpioOpencm3::SetHigh() {
  gpio_set(port_, pin_);
}

void GpioOpencm3::SetLow() {
  gpio_clear(port_, pin_);
}

void GpioOpencm3::Toggle() {
  gpio_toggle(port_, pin_);
}
