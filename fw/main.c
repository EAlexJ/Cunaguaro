#include <stdint.h>

#define VPRINTER_ADDR ((volatile uint8_t *)0x10000000UL)
#define HTIF_TOHOST ((volatile uint32_t *)0x11000000UL)

static void printer_putc(char c) { *VPRINTER_ADDR = (uint8_t)c; }

static void printer_puts(const char *str) {
  while (*str) {
    printer_putc(*str++);
  }
}

int main(void) {
  printer_puts("Hello, Bare Metal RISC-V World!\n");

  /* Halt execution */
  while (1) {
    auto code = 0;
    *HTIF_TOHOST = (uint32_t)((code << 1) | 1);
    __asm__ volatile("wfi");
  }

  return 0;
}