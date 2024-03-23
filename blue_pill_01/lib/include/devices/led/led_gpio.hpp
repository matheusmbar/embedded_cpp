#pragma once

#include <memory>

#include "gpio/gpio_interface.hpp"
#include "led/led_interface.hpp"

class LedGpio : public LedInterface {
 private:
  std::shared_ptr<GpioInterface> gpio_;
  LedMode mode_{LedMode::kOff};
  bool on_at_low_{false};

 public:
  explicit LedGpio(std::shared_ptr<GpioInterface> gpio, bool on_at_low = false);
  LedMode Get() const override;
  void SetOn() override;
  void SetOff() override;
  void Toggle() override;
};
