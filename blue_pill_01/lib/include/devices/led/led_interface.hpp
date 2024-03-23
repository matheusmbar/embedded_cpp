#pragma once

#include <stdint.h>

enum class LedMode { kOn, kOff, kBlink, kToggle };

struct LedCommand {
  LedMode mode{LedMode::kOff};
  uint32_t period_ms{1000};
};

class LedInterface {
 public:
  virtual ~LedInterface() = default;
  virtual LedMode Get() const = 0;
  virtual void SetOn() = 0;
  virtual void SetOff() = 0;
  virtual void Toggle() = 0;
};
