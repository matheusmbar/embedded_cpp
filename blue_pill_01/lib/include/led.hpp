#pragma once

#include <stdint.h>

enum class LedMode { kOn, kOff, kBlink, kToggle };

struct LedCommand {
  LedMode mode{LedMode::kOff};
  uint32_t period_ms{1000};
};
