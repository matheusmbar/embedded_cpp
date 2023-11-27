#pragma once

enum class GpioState { kLow, kHigh, kUnknown };

class GpioInterface {
 public:
  virtual GpioState Get() const = 0;
  virtual void Set(GpioState state) = 0;
  virtual void SetHigh() = 0;
  virtual void SetLow() = 0;
  virtual void Toggle() = 0;
};
