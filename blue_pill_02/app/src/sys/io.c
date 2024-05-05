#include <libopencm3/stm32/usart.h>

int local_putchar(char ptr) {
  usart_send_blocking(USART1, ptr);
  return 0;
}

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
