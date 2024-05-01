#include <gmock/gmock.h>

#include "snake.hpp"

class TestSnake : public testing::Test {
 public:
  Snake snake;
  Position pos;

  void SetUp() override {
    pos = snake.GetHead();
  }
};

TEST_F(TestSnake, StartZeroPoints) {
  EXPECT_EQ(0, snake.GetPoints());
}

TEST_F(TestSnake, StartNoColision) {
  EXPECT_FALSE(snake.ColisionDetected());
}

TEST_F(TestSnake, StartHeadIsNotZero) {
  EXPECT_NE(0, pos.x());
  EXPECT_NE(1, pos.y());
}
