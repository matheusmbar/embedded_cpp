
#include "test_cpp.h"

#include <string>

extern "C" {
    #include "test_c.h"
}


Test_CPP test_cpp(1001);

int main (void) {
    int check_c;
    int check_cpp;

    while(1) {
        check_c = test_c();
        if (check_c) {
            std::printf("check_c failed [%d]\n", check_c);
            return check_c;
        }
        else {
            std::printf("check_c succeeded\n");
        }

        if (1001 != test_cpp.getValue()) {
            return 1001;
        }

        check_cpp = test_cpp.checkGlobals();
        if (check_cpp) {
            std::printf("check_cpp failed [%d]\n", check_cpp);
            return check_cpp;
        }
        else {
            std::printf("check_cpp succeeded\n");
        }

        // printf calls _write() provided at syscalls.c
        std::printf("end of loop \n");
    }
    return 0;
}
