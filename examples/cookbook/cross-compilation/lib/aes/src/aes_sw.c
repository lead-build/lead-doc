#include "aes.h"

void aes_encrypt(const uint8_t key[16], const uint8_t in[16], uint8_t out[16]) {
    /* Placeholder for a software AES implementation */
    for (int i = 0; i < 16; i++) {
        out[i] = in[i] ^ key[i];
    }
}
