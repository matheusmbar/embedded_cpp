#pragma once

#include <gmock/gmock.h>

class Test1 : public testing::Test {
 protected:
  int val1 = 1000;
};
