
#include "test_cpp.h"

int global_cpp = 333;
int zero_cpp = 0;

Test_CPP::Test_CPP(int value) : value_(value) {}

int Test_CPP::getValue() {
    return value_;
}

void Test_CPP::setValue(int value) {
    value_ = value;
}

int Test_CPP::checkGlobals(void) {
    if (333 != global_cpp) {
        return -1;
    }
    if (zero_cpp) {
        return -2;
    }
    return 0;
}
