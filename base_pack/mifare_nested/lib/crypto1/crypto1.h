#include "../../lib/parity/parity.h"
#include "../nfclegacy/protocols/mifare_classic.h"
#include "../nfclegacy/protocols/crypto1.h"
#include "stddef.h"

#define LF_POLY_ODD (0x29CE5C)
#define LF_POLY_EVEN (0x870804)

#define SWAPENDIAN(x) \
    ((x) = ((x) >> 8 & 0xff00ff) | ((x) & 0xff00ff) << 8, (x) = (x) >> 16 | (x) << 16)
#define BEBIT(x, n) FURI_BIT(x, (n) ^ 24)

void nescrypto1_reset(Crypto1* crypto1);

void nescrypto1_init(Crypto1* crypto1, uint64_t key);

uint32_t nescrypto1_filter(uint32_t in);

uint8_t nescrypto1_bit(Crypto1* crypto1, uint8_t in, int is_encrypted);

uint8_t nescrypto1_byte(Crypto1* crypto1, uint8_t in, int is_encrypted);

uint32_t nescrypto1_word(Crypto1* crypto1, uint32_t in, int is_encrypted);

uint32_t nesprng_successor(uint32_t x, uint32_t n);

void nescrypto1_decrypt(
    Crypto1* crypto,
    uint8_t* encrypted_data,
    uint16_t encrypted_data_bits,
    uint8_t* decrypted_data);

void nescrypto1_encrypt(
    Crypto1* crypto,
    uint8_t* keystream,
    uint8_t* plain_data,
    uint16_t plain_data_bits,
    uint8_t* encrypted_data,
    uint8_t* encrypted_parity);