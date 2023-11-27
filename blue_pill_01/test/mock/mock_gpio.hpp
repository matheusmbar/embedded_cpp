#pragma once

#include <gmock/gmock.h>

#include "gpio/gpio_interface.hpp"

class MockGpio : public GpioInterface {
 public:
  MOCK_METHOD(GpioState, Get, (), (const));
  MOCK_METHOD(void, Set, (GpioState state));
  MOCK_METHOD(void, SetHigh, ());
  MOCK_METHOD(void, SetLow, ());
  MOCK_METHOD(void, Toggle, ());
};
