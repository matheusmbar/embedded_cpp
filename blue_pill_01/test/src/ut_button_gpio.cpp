#include <gmock/gmock.h>

#include <memory>

#include "devices/button/button_gpio.hpp"
#include "mock/mock_gpio.hpp"

using ::testing::Return;

class TestButtonGpio : public testing::Test {
 protected:
  std::shared_ptr<MockGpio> gpio;
  ButtonGpio btn;
  ButtonGpio btn_inverted;
  TestButtonGpio()
      : gpio(std::make_shared<MockGpio>()),
        btn(gpio, GpioState::kHigh),
        btn_inverted(gpio, GpioState::kLow) {}
};

TEST_F(TestButtonGpio, Pressed) {
  EXPECT_CALL(*gpio, Get()).WillOnce(Return(GpioState::kLow));
  ASSERT_EQ(false, btn.IsPressed());

  EXPECT_CALL(*gpio, Get()).WillOnce(Return(GpioState::kHigh));
  ASSERT_EQ(true, btn.IsPressed());
}

TEST_F(TestButtonGpio, PressedInverted) {
  EXPECT_CALL(*gpio, Get()).WillOnce(Return(GpioState::kLow));
  ASSERT_EQ(true, btn_inverted.IsPressed());

  EXPECT_CALL(*gpio, Get()).WillOnce(Return(GpioState::kHigh));
  ASSERT_EQ(false, btn_inverted.IsPressed());
}

TEST_F(TestButtonGpio, GpioUnknownReturnsFalse) {
  EXPECT_CALL(*gpio, Get()).WillOnce(Return(GpioState::kUnknown));
  ASSERT_EQ(false, btn.IsPressed());

  EXPECT_CALL(*gpio, Get()).WillOnce(Return(GpioState::kUnknown));
  ASSERT_EQ(false, btn_inverted.IsPressed());
}

TEST_F(TestButtonGpio, UnknownPressedAlwaysFalse) {
  ButtonGpio btn_unk(gpio, GpioState::kUnknown);
  ASSERT_EQ(false, btn_unk.IsPressed());
}
