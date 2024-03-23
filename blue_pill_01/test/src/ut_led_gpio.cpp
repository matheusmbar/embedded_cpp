#include <gmock/gmock.h>

#include <memory>

#include "devices/led/led_gpio.hpp"
#include "mock/mock_gpio.hpp"

using ::testing::AnyNumber;

class TestLedGpio : public testing::Test {
 protected:
  std::shared_ptr<MockGpio> gpio;
  LedGpio led;
  LedGpio led_inverted;
  TestLedGpio() : gpio(std::make_shared<MockGpio>()), led(gpio), led_inverted(gpio, true) {}
};

TEST_F(TestLedGpio, SetOn) {
  EXPECT_CALL(*gpio, SetHigh());
  led.SetOn();
}

TEST_F(TestLedGpio, SetOff) {
  EXPECT_CALL(*gpio, SetLow());
  led.SetOff();
}

TEST_F(TestLedGpio, Toggle) {
  EXPECT_CALL(*gpio, Toggle());
  led.Toggle();
}

TEST_F(TestLedGpio, GetOnOff) {
  EXPECT_CALL(*gpio, SetLow());
  EXPECT_CALL(*gpio, SetHigh());
  led.SetOff();
  ASSERT_EQ(LedMode::kOff, led.Get());
  led.SetOn();
  ASSERT_EQ(LedMode::kOn, led.Get());
}

TEST_F(TestLedGpio, GetToggle) {
  EXPECT_CALL(*gpio, SetLow());
  EXPECT_CALL(*gpio, Toggle()).Times(2);

  led.SetOff();
  led.Toggle();
  ASSERT_EQ(LedMode::kOn, led.Get());
  led.Toggle();
  ASSERT_EQ(LedMode::kOff, led.Get());
}

TEST_F(TestLedGpio, OnAtLow_SetOn) {
  EXPECT_CALL(*gpio, SetLow());
  led_inverted.SetOn();
}

TEST_F(TestLedGpio, OnAtLow_SetOff) {
  EXPECT_CALL(*gpio, SetHigh());
  led_inverted.SetOff();
}

TEST_F(TestLedGpio, OnAtLow_Toggle) {
  EXPECT_CALL(*gpio, Toggle());
  led_inverted.Toggle();
}

TEST_F(TestLedGpio, OnAtLow_GetOnOff) {
  EXPECT_CALL(*gpio, SetHigh());
  EXPECT_CALL(*gpio, SetLow());
  led_inverted.SetOff();
  ASSERT_EQ(LedMode::kOff, led_inverted.Get());
  led_inverted.SetOn();
  ASSERT_EQ(LedMode::kOn, led_inverted.Get());
}

TEST_F(TestLedGpio, OnAtLow_GetToggle) {
  EXPECT_CALL(*gpio, SetHigh());
  EXPECT_CALL(*gpio, Toggle()).Times(2);

  led_inverted.SetOff();
  led_inverted.Toggle();
  ASSERT_EQ(LedMode::kOn, led_inverted.Get());
  led_inverted.Toggle();
  ASSERT_EQ(LedMode::kOff, led_inverted.Get());
}
