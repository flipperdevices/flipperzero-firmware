#include "mrtd_helpers.h"

#include <mbedtls/sha1.h>
#include <mbedtls/des.h>

static inline unsigned char *ucstr(const char *str) { return (unsigned char *)str; }

uint8_t mrtd_bac_check_digit(const uint8_t* input, const uint8_t length) {
    const uint8_t num_weights = 3;
    uint8_t weights[] = {7, 3, 1};
    uint8_t check_digit = 0;
    uint8_t idx;

    for(uint8_t i=0; i<length; ++i) {
        uint8_t c = input[i];
        if(c >= 'A' && c <= 'Z') {
            idx = c - 'A' + 10;
        } else if(c >= 'a' && c <= 'z') {
            idx = c - 'a' + 10;
        } else if(c >= '0' && c <= '9') {
            idx = c - '0';
        } else {
            idx = 0;
        }
        check_digit = (check_digit + idx * weights[i%num_weights]) % 10;
    }
    return check_digit;
}

void mrtd_print_date(uint8_t* output, MrtdDate* date) {
    output[0] = (date->year / 10) + '0';
    output[1] = (date->year % 10) + '0';
    output[2] = (date->month / 10) + '0';
    output[3] = (date->month % 10) + '0';
    output[4] = (date->day / 10) + '0';
    output[5] = (date->day % 10) + '0';
}

bool mrtd_bac_get_kmrz(MrtdAuthData* auth, uint8_t* output, uint8_t output_size) {
    uint8_t idx = 0;
    uint8_t docnr_length = strlen(auth->doc_number);
    uint8_t cd_idx = 0;
    if(output_size < docnr_length + 16) {
        return false;
    }

    cd_idx = idx;
    memcpy(output+idx, auth->doc_number, docnr_length);
    idx += docnr_length;
    if(docnr_length < 9) {
        memset(output+idx, '<', 9-docnr_length);
        idx += 9-docnr_length;
    }

    output[idx++] = mrtd_bac_check_digit(output+cd_idx, docnr_length) + '0';

    cd_idx = idx;
    mrtd_print_date(output+idx, &auth->birth_date);
    idx += 6;
    output[idx++] = mrtd_bac_check_digit(output+cd_idx, 6) + '0';

    cd_idx = idx;
    mrtd_print_date(output+idx, &auth->expiry_date);
    idx += 6;
    output[idx++] = mrtd_bac_check_digit(output+cd_idx, 6) + '0';

    output[idx++] = '\x00';
    return true;
}

bool mrtd_bac_keys(const uint8_t kseed[16], uint8_t ksenc[16], uint8_t ksmac[16]) {
    uint8_t hash[20];
    mbedtls_sha1_context ctx;
    mbedtls_sha1_init(&ctx);

    do {
        for(uint8_t i=1; i<=2; ++i) {
            if(mbedtls_sha1_starts(&ctx)) break;
            if(mbedtls_sha1_update(&ctx, kseed, 16)) break;
            if(mbedtls_sha1_update(&ctx, ucstr("\x00\x00\x00"), 3)) break;
            if(mbedtls_sha1_update(&ctx, &i, 1)) break;
            if(mbedtls_sha1_finish(&ctx, hash)) break;

            switch(i) {
                case 1:
                    memcpy(ksenc, hash, 16);
                    mbedtls_des_key_set_parity(ksenc);
                    mbedtls_des_key_set_parity(ksenc+8);
                    break;
                case 2:
                    memcpy(ksmac, hash, 16);
                    mbedtls_des_key_set_parity(ksmac);
                    mbedtls_des_key_set_parity(ksmac+8);
                    break;
            }
        }
    } while(false);

    mbedtls_sha1_free(&ctx);
    return true;
}
