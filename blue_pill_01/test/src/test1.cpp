#include "test1.hpp"

#include <gmock/gmock.h>

#include "mymath.hpp"

TEST_F(Test1, checkSum) {
  EXPECT_EQ(4, MyMath::Sum(2, 2));
  EXPECT_EQ(0, MyMath::Sum(2, -2));
  EXPECT_EQ(2 * val1, MyMath::Sum(val1, val1));
}

TEST_F(Test1, standard) {
  EXPECT_GE(__cplusplus, 201703);
}
