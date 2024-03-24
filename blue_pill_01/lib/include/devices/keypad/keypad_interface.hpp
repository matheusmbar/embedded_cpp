#pragma once

#include <etl/vector.h>

#include "devices/button/button_interface.hpp"

enum class Key { kUp, kLeft, kDown, kRight, kCenter, kNone };

class KeypadInterface {
 public:
  virtual ~KeypadInterface() = default;
  virtual const etl::ivector<Key>& GetPressed() = 0;
  virtual bool Up() const = 0;
  virtual bool Left() const = 0;
  virtual bool Down() const = 0;
  virtual bool Right() const = 0;
  virtual bool Center() const = 0;
};
