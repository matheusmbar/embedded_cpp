#pragma once

#include <etl/vector.h>

#include <memory>

#include "devices/keypad/keypad_interface.hpp"

class KeypadButton : public KeypadInterface {
 public:
  KeypadButton(const std::shared_ptr<ButtonInterface> btn_up_,
               const std::shared_ptr<ButtonInterface> btn_left_,
               const std::shared_ptr<ButtonInterface> btn_down_,
               const std::shared_ptr<ButtonInterface> btn_right_,
               const std::shared_ptr<ButtonInterface> btn_center_);

  const etl::ivector<Key>& GetPressed() override;

  bool Up() const override;
  bool Left() const override;
  bool Down() const override;
  bool Right() const override;
  bool Center() const override;

 private:
  const std::shared_ptr<ButtonInterface> btn_up_;
  const std::shared_ptr<ButtonInterface> btn_left_;
  const std::shared_ptr<ButtonInterface> btn_down_;
  const std::shared_ptr<ButtonInterface> btn_right_;
  const std::shared_ptr<ButtonInterface> btn_center_;
  etl::vector<Key, 5> vec;
};
