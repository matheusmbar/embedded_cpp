#include "devices/led/led_gpio.hpp"

LedGpio::LedGpio(std::shared_ptr<GpioInterface> gpio, bool on_at_low)
    : gpio_(gpio), on_at_low_(on_at_low) {}

LedMode LedGpio::Get() const {
  return mode_;
}

void LedGpio::SetOn() {
  if (on_at_low_) {
    gpio_->SetLow();
  } else {
    gpio_->SetHigh();
  }
  mode_ = LedMode::kOn;
}

void LedGpio::SetOff() {
  if (on_at_low_) {
    gpio_->SetHigh();
  } else {
    gpio_->SetLow();
  }
  mode_ = LedMode::kOff;
}

void LedGpio::Toggle() {
  gpio_->Toggle();
  if (mode_ == LedMode::kOff) {
    mode_ = LedMode::kOn;
  } else {
    mode_ = LedMode::kOff;
  }
}
