#include "main.hpp"

int _write(int file, char* ptr, int len) {
  int written = 0;
  for (; len != 0; --len) {
    if (local_putchar(*ptr++)) {
      return -1;
    }
    ++written;
  }
  return written;
}
