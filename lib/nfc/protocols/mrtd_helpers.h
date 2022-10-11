#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    uint8_t year;
    uint8_t month;
    uint8_t day;
} MrtdDate;

// NULL terminated document ID
#define MRTD_DOCNR_MAX_LENGTH 21

typedef enum {
    MrtdAuthMethodNone,
    MrtdAuthMethodAny,
    MrtdAuthMethodBac,
    MrtdAuthMethodPace,
} MrtdAuthMethod;

typedef struct {
    MrtdAuthMethod method;

    // BAC input fields
    MrtdDate birth_date;
    MrtdDate expiry_date;
    char doc_number[MRTD_DOCNR_MAX_LENGTH];

    //TODO: PACE
} MrtdAuthData;

uint8_t mrtd_bac_check_digit(const char* input, const uint8_t length);

//TODO: swap order, all other functions have output last
void mrtd_print_date(char* output, MrtdDate* date);

bool mrtd_bac_get_kmrz(MrtdAuthData* auth, char* output, uint8_t output_size);

bool mrtd_bac_keys_from_seed(const uint8_t* kseed, uint8_t* ksenc, uint8_t* ksmac);

bool mrtd_bac_keys(MrtdAuthData* auth, uint8_t ksenc[16], uint8_t ksmac[16]);

bool mrtd_bac_encrypt(const uint8_t* data, size_t data_length, uint8_t* key, uint8_t* output);

bool mrtd_bac_mac(const uint8_t* data, size_t data_length, uint8_t* key, uint8_t* output);

bool mrtd_bac_padded_mac(const uint8_t* data, size_t data_length, uint8_t* key, uint8_t* output);

bool mrtd_bac_decrypt(const uint8_t* data, size_t data_length, uint8_t* key, uint8_t* output);

bool mrtd_bac_decrypt_verify(const uint8_t* data, size_t data_length, uint8_t* key_enc, uint8_t* key_mac, uint8_t* output);

static __inline uint64_t mrtd_ssc_from_data(const uint8_t* rnd_ic, const uint8_t* rnd_ifd) {
#if _BYTE_ORDER == _LITTLE_ENDIAN
    return
        (((uint64_t)rnd_ic[4] << 56) & 0xff00000000000000) |
        (((uint64_t)rnd_ic[5] << 48) & 0x00ff000000000000) |
        (((uint64_t)rnd_ic[6] << 40) & 0x0000ff0000000000) |
        (((uint64_t)rnd_ic[7] << 32) & 0x000000ff00000000) |
        (((uint64_t)rnd_ifd[4] << 24) & 0x00000000ff000000) |
        (((uint64_t)rnd_ifd[5] << 16) & 0x0000000000ff0000) |
        (((uint64_t)rnd_ifd[6] << 8)  & 0x000000000000ff00) |
        (((uint64_t)rnd_ifd[7])       & 0x00000000000000ff);
#else
#error Using untested code, please verify first!
    return (*((uint64_t*)(rnd_ic + 4)) & 0xffffffff) +
        (*((uint64_t*)(rnd_ifd + 4)) * 0x100000000);
#endif
}
