#pragma once

#include "devices/button/button_interface.hpp"
#include "memory"

class ButtonGpio : public ButtonInterface {
 public:
  explicit ButtonGpio(std::shared_ptr<GpioInterface> gpio, GpioState pressed_state);
  bool IsPressed() const override;

 private:
  std::shared_ptr<GpioInterface> gpio_;
  GpioState pressed_state_;
};
