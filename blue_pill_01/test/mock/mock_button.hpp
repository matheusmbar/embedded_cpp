#pragma once

#include <gmock/gmock.h>

#include "devices/button/button_interface.hpp"

class MockButton : public ButtonInterface {
 public:
  MOCK_METHOD(bool, IsPressed, (), (const));
};
