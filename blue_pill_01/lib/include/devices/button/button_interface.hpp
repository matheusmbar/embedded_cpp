#pragma once

#include "devices/gpio/gpio_interface.hpp"

class ButtonInterface {
 public:
  virtual ~ButtonInterface() = default;
  virtual bool IsPressed() const = 0;
};
