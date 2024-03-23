#pragma once

#include <cstdint>

#include "devices/gpio/gpio_interface.hpp"

class GpioOpencm3 : public GpioInterface {
 public:
  GpioOpencm3(GpioFunction function, uint32_t port, uint16_t pin);
  ~GpioOpencm3() override = default;
  GpioState Get() const override;
  void Set(GpioState state) override;
  void SetHigh() override;
  void SetLow() override;
  void Toggle() override;

 private:
  GpioFunction function_;
  uint32_t port_;
  uint16_t pin_;
};
