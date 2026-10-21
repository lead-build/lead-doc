#include "bsp.h"
#include <stdio.h>

void bsp_init(void) {
}

void bsp_puts(const char *str) {
    fputs(str, stdout);
}
