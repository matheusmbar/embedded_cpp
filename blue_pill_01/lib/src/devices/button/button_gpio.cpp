#include "devices/button/button_gpio.hpp"

ButtonGpio::ButtonGpio(std::shared_ptr<GpioInterface> gpio, GpioState pressed_state)
    : gpio_(gpio), pressed_state_(pressed_state) {}

bool ButtonGpio::IsPressed() const {
  if (pressed_state_ == GpioState::kUnknown) {
    return false;
  }
  return gpio_->Get() == pressed_state_;
}
