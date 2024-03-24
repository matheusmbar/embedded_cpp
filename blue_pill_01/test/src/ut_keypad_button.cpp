#include <gmock/gmock.h>

#include <memory>

#include "devices/keypad/keypad_button.hpp"
#include "mock/mock_button.hpp"

using ::testing::Return;
using ::testing::UnorderedElementsAre;

class TestKeypadButton : public testing::Test {
 protected:
  std::shared_ptr<MockButton> btn_up;
  std::shared_ptr<MockButton> btn_left;
  std::shared_ptr<MockButton> btn_down;
  std::shared_ptr<MockButton> btn_right;
  std::shared_ptr<MockButton> btn_center;

  KeypadButton keypad;

  TestKeypadButton()
      : btn_up(std::make_shared<MockButton>()),
        btn_left(std::make_shared<MockButton>()),
        btn_down(std::make_shared<MockButton>()),
        btn_right(std::make_shared<MockButton>()),
        btn_center(std::make_shared<MockButton>()),
        keypad(btn_up, btn_left, btn_down, btn_right, btn_center) {}
};

TEST_F(TestKeypadButton, KeyUp) {
  EXPECT_CALL(*btn_up, IsPressed()).WillOnce(Return(true));
  EXPECT_EQ(true, keypad.Up());

  EXPECT_CALL(*btn_up, IsPressed()).WillOnce(Return(false));
  EXPECT_EQ(false, keypad.Up());
}

TEST_F(TestKeypadButton, KeyLeft) {
  EXPECT_CALL(*btn_left, IsPressed()).WillOnce(Return(true));
  EXPECT_EQ(true, keypad.Left());

  EXPECT_CALL(*btn_left, IsPressed()).WillOnce(Return(false));
  EXPECT_EQ(false, keypad.Left());
}

TEST_F(TestKeypadButton, KeyDown) {
  EXPECT_CALL(*btn_down, IsPressed()).WillOnce(Return(true));
  EXPECT_EQ(true, keypad.Down());

  EXPECT_CALL(*btn_down, IsPressed()).WillOnce(Return(false));
  EXPECT_EQ(false, keypad.Down());
}

TEST_F(TestKeypadButton, KeyRight) {
  EXPECT_CALL(*btn_right, IsPressed()).WillOnce(Return(true));
  EXPECT_EQ(true, keypad.Right());

  EXPECT_CALL(*btn_right, IsPressed()).WillOnce(Return(false));
  EXPECT_EQ(false, keypad.Right());
}

TEST_F(TestKeypadButton, KeyCenter) {
  EXPECT_CALL(*btn_center, IsPressed()).WillOnce(Return(true));
  EXPECT_EQ(true, keypad.Center());

  EXPECT_CALL(*btn_center, IsPressed()).WillOnce(Return(false));
  EXPECT_EQ(false, keypad.Center());
}

TEST_F(TestKeypadButton, PressedNone) {
  ON_CALL(*btn_up, IsPressed()).WillByDefault(Return(false));
  ON_CALL(*btn_left, IsPressed()).WillByDefault(Return(false));
  ON_CALL(*btn_down, IsPressed()).WillByDefault(Return(false));
  ON_CALL(*btn_right, IsPressed()).WillByDefault(Return(false));
  ON_CALL(*btn_center, IsPressed()).WillByDefault(Return(false));
  auto& pressed = keypad.GetPressed();

  EXPECT_TRUE(pressed.empty());
}

TEST_F(TestKeypadButton, PressedAll) {
  ON_CALL(*btn_up, IsPressed()).WillByDefault(Return(true));
  ON_CALL(*btn_left, IsPressed()).WillByDefault(Return(true));
  ON_CALL(*btn_down, IsPressed()).WillByDefault(Return(true));
  ON_CALL(*btn_right, IsPressed()).WillByDefault(Return(true));
  ON_CALL(*btn_center, IsPressed()).WillByDefault(Return(true));

  auto& pressed = keypad.GetPressed();

  EXPECT_THAT(pressed,
              UnorderedElementsAre(Key::kUp, Key::kLeft, Key::kDown, Key::kRight, Key::kCenter));
}

TEST_F(TestKeypadButton, PressedSome) {
  ON_CALL(*btn_left, IsPressed()).WillByDefault(Return(true));
  ON_CALL(*btn_down, IsPressed()).WillByDefault(Return(true));
  ON_CALL(*btn_center, IsPressed()).WillByDefault(Return(true));

  auto& pressed = keypad.GetPressed();

  EXPECT_THAT(pressed, UnorderedElementsAre(Key::kLeft, Key::kDown, Key::kCenter));
}
