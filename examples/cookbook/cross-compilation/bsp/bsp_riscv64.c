#include "bsp.h"
#include <stdint.h>

/* Transmit register of a hypothetical UART */
#define UART_TX ((volatile uint8_t *)0x10000000)

void bsp_init(void) {
}

void bsp_puts(const char *str) {
    while (*str) {
        *UART_TX = *str++;
    }
}
