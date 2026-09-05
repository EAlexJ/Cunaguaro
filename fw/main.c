#include <stdint.h>

#define UART_BASE 0x10000000UL
#define UART_THR                                                               \
  ((volatile uint8_t *)(UART_BASE + 0x00)) /* Transmitter Holding Register */
#define UART_LSR                                                               \
  ((volatile uint8_t *)(UART_BASE + 0x05)) /* Line Status Register */
#define UART_LSR_TX_EMPTY (1 << 5)

static void uart_putc(char c) {
  /* Wait until the transmit buffer is empty */
  while (!(*UART_LSR & UART_LSR_TX_EMPTY))
    ;
  *UART_THR = (uint8_t)c;
}

static void uart_puts(const char *str) {
  while (*str) {
    if (*str == '\n') {
      uart_putc('\r');
    }
    uart_putc(*str++);
  }
}

int main(void) {
  uart_puts("Hello, Bare Metal RISC-V World!\n");

  /* Halt */
  while (1) {
    __asm__ volatile("wfi");
  }

  return 0;
}