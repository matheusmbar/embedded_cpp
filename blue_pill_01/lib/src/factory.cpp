#include "factory.hpp"

#include <memory>

#include "devices/gpio/gpio_interface.hpp"
#include "devices/gpio/gpio_opencm3.hpp"

std::shared_ptr<GpioInterface> MakeGpio(GpioFunction function, uint32_t port, uint16_t pin) {
  return std::make_shared<GpioOpencm3>(GpioFunction::kOutput, port, pin);
}

std::shared_ptr<LedInterface> MakeLed(std::shared_ptr<GpioInterface> gpio, bool on_at_low) {
  return std::make_shared<LedGpio>(gpio, on_at_low);
}

std::shared_ptr<ButtonInterface> MakeButton(std::shared_ptr<GpioInterface> gpio,
                                            GpioState pressed_state) {
  return std::make_shared<ButtonGpio>(gpio, pressed_state);
}

std::shared_ptr<KeypadInterface> MakeKeypad(const std::shared_ptr<ButtonInterface> btn_up,
                                            const std::shared_ptr<ButtonInterface> btn_left,
                                            const std::shared_ptr<ButtonInterface> btn_down,
                                            const std::shared_ptr<ButtonInterface> btn_right,
                                            const std::shared_ptr<ButtonInterface> btn_center) {
  return std::make_shared<KeypadButton>(btn_up, btn_left, btn_down, btn_right, btn_center);
}