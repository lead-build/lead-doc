#pragma once

#include <stdint.h>

void aes_encrypt(const uint8_t key[16], const uint8_t in[16], uint8_t out[16]);
