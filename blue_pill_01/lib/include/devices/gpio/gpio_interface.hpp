#pragma once

enum class GpioState { kLow, kHigh, kUnknown };

enum class GpioFunction { kInput, kInputPullDown, kInputPullUp, kOutput };

class GpioInterface {
 public:
  virtual ~GpioInterface() = default;
  virtual GpioState Get() const = 0;
  virtual void Set(GpioState state) = 0;
  virtual void SetHigh() = 0;
  virtual void SetLow() = 0;
  virtual void Toggle() = 0;
};
