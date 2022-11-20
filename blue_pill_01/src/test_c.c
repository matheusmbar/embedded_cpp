
#include "test_c.h"

int global_c = 123;
int zero_c = 0;

int test_c (void) {
    if (123 != global_c) {
        return -1;
    }
    if (zero_c) {
        return -2;
    }
    return 0;
}
