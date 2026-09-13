#include <stdint.h>

#define VPRINTER_ADDR ((volatile uint8_t *)0x10000000UL)
#define SYS_WRITE 64
extern volatile uint64_t tohost;
static void printer_putc(char c) { *VPRINTER_ADDR = (uint8_t)c; }

static void printer_puts(const char *str) {
  while (*str) {
    printer_putc(*str++);
  }
}
static volatile uint64_t htif_payload[8] __attribute__((aligned(8)));

static void htif_write(const char *buf, uint32_t len) {
  htif_payload[0] = SYS_WRITE;
  htif_payload[1] = 1;
  htif_payload[2] = (uint64_t)(uintptr_t)buf;
  htif_payload[3] = len;
  tohost = (uint64_t)(uintptr_t)&htif_payload;
}
int main(void) {
  printer_puts("Hello, Bare Metal RISC-V World!\n");
  const char msg[] = "Hello, Bare Metal RISC-V World (using tohost)!\n";
  htif_write(msg, sizeof(msg) - 1);
  /* Halt execution */
  while (1) {
    auto code = 0;
    tohost = (uint64_t)((code << 1) | 1);
    __asm__ volatile("wfi");
  }

  return 0;
}