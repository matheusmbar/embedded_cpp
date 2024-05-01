
#include "sys/checks/test_cpp.hpp"

extern "C" {
#include "sys/checks/test_c.h"
}

int global_cpp = 333;
int zero_cpp = 0;

Test_CPP::Test_CPP(int value) : value_(value) {}

int Test_CPP::getValue() const {
  return value_;
}

void Test_CPP::setValue(int value) {
  value_ = value;
}

int Test_CPP::checkGlobals(void) const {
  if (333 != global_cpp) {
    return -1;
  }
  if (zero_cpp) {
    return -2;
  }
  return 0;
}

const Test_CPP test_cpp(1001);

int check_inits() {
  if (auto check_c = test_c(); check_c) {
    // std::printf("check_c failed [%d]\n", check_c);
    return check_c;
  }

  if (1001 != test_cpp.getValue()) {
    return 1001;
  }

  if (auto check_cpp = test_cpp.checkGlobals(); check_cpp) {
    // std::printf("check_cpp failed [%d]\n", check_cpp);
    return check_cpp;
  }
  return 0;
}
