#include "devices/keypad/keypad_button.hpp"

KeypadButton::KeypadButton(const std::shared_ptr<ButtonInterface> btn_up,
                           const std::shared_ptr<ButtonInterface> btn_left,
                           const std::shared_ptr<ButtonInterface> btn_down,
                           const std::shared_ptr<ButtonInterface> btn_right,
                           const std::shared_ptr<ButtonInterface> btn_center)
    : btn_up_(btn_up),
      btn_left_(btn_left),
      btn_down_(btn_down),
      btn_right_(btn_right),
      btn_center_(btn_center) {}

const etl::ivector<Key>& KeypadButton::GetPressed() {
  vec.clear();
  if (Up()) {
    vec.push_back(Key::kUp);
  }
  if (Left()) {
    vec.push_back(Key::kLeft);
  }
  if (Down()) {
    vec.push_back(Key::kDown);
  }
  if (Right()) {
    vec.push_back(Key::kRight);
  }
  if (Center()) {
    vec.push_back(Key::kCenter);
  }
  return vec;
}

bool KeypadButton::Up() const {
  return btn_up_->IsPressed();
}

bool KeypadButton::Left() const {
  return btn_left_->IsPressed();
}

bool KeypadButton::Down() const {
  return btn_down_->IsPressed();
}

bool KeypadButton::Right() const {
  return btn_right_->IsPressed();
}

bool KeypadButton::Center() const {
  return btn_center_->IsPressed();
}
