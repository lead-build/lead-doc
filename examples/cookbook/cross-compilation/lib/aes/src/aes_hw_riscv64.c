#include "aes.h"

/* Registers of a hypothetical AES accelerator */
#define AES_KEY ((volatile uint8_t *)0x10010000)
#define AES_DATA_IN ((volatile uint8_t *)0x10010010)
#define AES_DATA_OUT ((volatile uint8_t *)0x10010020)
#define AES_CTRL ((volatile uint32_t *)0x10010030)
#define AES_CTRL_START 0x1
#define AES_CTRL_BUSY 0x2

void aes_encrypt(const uint8_t key[16], const uint8_t in[16], uint8_t out[16]) {
    for (int i = 0; i < 16; i++) {
        AES_KEY[i] = key[i];
        AES_DATA_IN[i] = in[i];
    }
    *AES_CTRL = AES_CTRL_START;
    while (*AES_CTRL & AES_CTRL_BUSY) {
    }
    for (int i = 0; i < 16; i++) {
        out[i] = AES_DATA_OUT[i];
    }
}
