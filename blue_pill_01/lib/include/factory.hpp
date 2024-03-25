#pragma once

#include <memory>

#include "devices/button/button_gpio.hpp"
#include "devices/button/button_interface.hpp"
#include "devices/gpio/gpio_interface.hpp"
#include "devices/gpio/gpio_opencm3.hpp"
#include "devices/keypad/keypad_button.hpp"
#include "devices/keypad/keypad_interface.hpp"
#include "devices/led/led_gpio.hpp"
#include "devices/led/led_interface.hpp"

std::shared_ptr<GpioInterface> MakeGpio(GpioFunction function, uint32_t port, uint16_t pin);
std::shared_ptr<LedInterface> MakeLed(std::shared_ptr<GpioInterface> gpio, bool on_at_low = false);
std::shared_ptr<ButtonInterface> MakeButton(std::shared_ptr<GpioInterface> gpio,
                                            GpioState pressed_state);
std::shared_ptr<KeypadInterface> MakeKeypad(const std::shared_ptr<ButtonInterface> btn_up,
                                            const std::shared_ptr<ButtonInterface> btn_left,
                                            const std::shared_ptr<ButtonInterface> btn_down,
                                            const std::shared_ptr<ButtonInterface> btn_right,
                                            const std::shared_ptr<ButtonInterface> btn_center);
