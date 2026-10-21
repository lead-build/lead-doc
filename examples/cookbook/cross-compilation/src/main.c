#include "aes.h"
#include "bsp.h"

int main() {
    static const uint8_t key[16] = "0123456789abcdef";
    static const uint8_t block[16] = "Hello, platform!";
    uint8_t encrypted[16];

    bsp_init();
    aes_encrypt(key, block, encrypted);
    bsp_puts("Encrypted a block\n");
    return 0;
}
