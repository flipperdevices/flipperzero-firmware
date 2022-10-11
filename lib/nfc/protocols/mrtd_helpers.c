#include "mrtd_helpers.h"

#include <stdio.h> //TODO: remove
#include <stdlib.h>

#include <mbedtls/sha1.h>
#include <mbedtls/des.h>

static inline unsigned char *ucstr(const char *str) { return (unsigned char *)str; }

uint8_t mrtd_bac_check_digit(const char* input, const uint8_t length) {
    const uint8_t num_weights = 3;
    uint8_t weights[] = {7, 3, 1};
    uint8_t check_digit = 0;
    uint8_t idx;

    for(uint8_t i=0; i<length; ++i) {
        char c = input[i];
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

void mrtd_print_date(char* output, MrtdDate* date) {
    output[0] = (date->year / 10) + '0';
    output[1] = (date->year % 10) + '0';
    output[2] = (date->month / 10) + '0';
    output[3] = (date->month % 10) + '0';
    output[4] = (date->day / 10) + '0';
    output[5] = (date->day % 10) + '0';
}

bool mrtd_bac_get_kmrz(MrtdAuthData* auth, char* output, uint8_t output_size) {
    uint8_t idx = 0;
    uint8_t docnr_length = strlen(auth->doc_number);
    uint8_t cd_idx = 0;
    if(output_size < docnr_length + 16) {
        return false;
    }

    cd_idx = idx;
    for(uint8_t i=0; i<docnr_length; ++i) {
        char c = auth->doc_number[i];
        if(c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        output[idx++] = c;
    }

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

bool mrtd_bac_keys_from_seed(const uint8_t kseed[16], uint8_t ksenc[16], uint8_t ksmac[16]) {
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

bool mrtd_bac_keys(MrtdAuthData* auth, uint8_t ksenc[16], uint8_t ksmac[16]) {
    uint8_t kmrz_max_length = MRTD_DOCNR_MAX_LENGTH + 16;
    char kmrz[kmrz_max_length];
    if(!mrtd_bac_get_kmrz(auth, kmrz, kmrz_max_length)) {
        return false;
    }

    printf("kmrz: %s\r\n", kmrz); //TODO: remove

    uint8_t hash[20];
    mbedtls_sha1((uint8_t*)kmrz, strlen(kmrz), hash);

    if(!mrtd_bac_keys_from_seed(hash, ksenc, ksmac)) {
        return false;
    }

    return true;
}

//NOTE: output size will be ((data_length+8)/8)*8
bool mrtd_bac_encrypt(const uint8_t* data, size_t data_length, uint8_t* key, uint8_t* output) {
    uint8_t IV[8] = "\x00\x00\x00\x00\x00\x00\x00\x00";

    mbedtls_des3_context ctx;
    mbedtls_des3_init(&ctx);
    mbedtls_des3_set2key_enc(&ctx, key);
    if(mbedtls_des3_crypt_cbc(&ctx, MBEDTLS_DES_ENCRYPT, data_length, IV, data, output)) {
        return false;
    }
    mbedtls_des3_free(&ctx);

    return true;
}

bool mrtd_bac_decrypt(const uint8_t* data, size_t data_length, uint8_t* key, uint8_t* output) {
    uint8_t IV[8] = "\x00\x00\x00\x00\x00\x00\x00\x00";

    mbedtls_des3_context ctx;
    mbedtls_des3_init(&ctx);
    mbedtls_des3_set2key_dec(&ctx, key);
    if(mbedtls_des3_crypt_cbc(&ctx, MBEDTLS_DES_DECRYPT, data_length, IV, data, output)) {
        return false;
    }
    mbedtls_des3_free(&ctx);

    return true;
}

bool mrtd_bac_decrypt_verify(const uint8_t* data, size_t data_length, uint8_t* key_enc, uint8_t* key_mac, uint8_t* output) {
    mrtd_bac_decrypt(data, data_length - 8, key_enc, output);

    uint8_t mac_calc[8];
    mrtd_bac_padded_mac(data, data_length - 8, key_mac, mac_calc);

    if(memcmp(mac_calc, data + data_length - 8, 8)) {
        printf( "MAC failed\r\n");
        for(uint8_t i=0; i<8; ++i) {
            printf("%02X <=> %02X\r\n", mac_calc[i], data[data_length - 8 + i]);
        }
        return false;
    }
    return true;
}

bool mrtd_bac_decrypt_verify_sm(const uint8_t* data, size_t data_length, uint8_t* key_enc, uint8_t* key_mac, uint64_t ssc, uint8_t* output, uint16_t* ret_code) {
    // Message: [DO'85 or DO'87] || [DO'99] || DO'8E
    // Lengths:      Var            1+1+2=4    1+1+8=10

    *ret_code = data[data_length - 10 - 2] <<8 | data[data_length - 10 - 1];
    //ntohs(data + data_length - 10 - 2);

    if(data[0] == 0x87) {
        uint8_t do87_length = data[1] - 1;
        mrtd_bac_decrypt(data + 3, do87_length, key_enc, output);
        printf("Decrypted: "); for(uint8_t i=0; i<do87_length; ++i) printf("%02X ", output[i]); printf("\r\n");

        //TODO: mrtd_bac_unpad
        int padidx;
        for(padidx=do87_length-1; padidx>=0; --padidx) {
            if(output[padidx] == 0x00) {
                continue;
            } else if(output[padidx] == 0x80) {
                break;
            } else {
                printf("Invalid padding\r\n");
                return false;
            }
        }
        printf("           ");
        for(int i=0; i<padidx; ++i) {
            printf("   ");
        }
        printf("^^\r\n");
        printf("Pad starts at: %d\r\n", padidx);

        //TODO: return padidx-1 as output length
    }

    mrtd_bac_mac_ctx ctx;
    mrtd_bac_mac_init(&ctx, key_mac);
    uint64_t ssc_n = htonll(ssc);
    mrtd_bac_mac_update(&ctx, (uint8_t*)&ssc_n, 8);
    mrtd_bac_mac_update(&ctx, data, data_length - 10); // 10 = len(DO'8E) = len(header + length + MAC) = 1 + 1 + 8
    uint8_t mac_calc[8];
    mrtd_bac_mac_finalize(&ctx, mac_calc);

    if(memcmp(mac_calc, data + data_length - 8, 8)) {
        printf( "SM MAC failed\r\n");
        for(uint8_t i=0; i<8; ++i) {
            printf("%02X <=> %02X\r\n", mac_calc[i], data[data_length - 8 + i]);
        }
        return false;
    }
    return true;
}

bool mrtd_bac_mac_init(mrtd_bac_mac_ctx* ctx, uint8_t key[16]) {
    mbedtls_des_init(&ctx->des);
    mbedtls_des_setkey_enc(&ctx->des, key);
    memset(ctx->mac, 0, 8);
    ctx->idx_in = 0;
    memcpy(ctx->key, key, 16);
    return true;
}

bool mrtd_bac_mac_update(mrtd_bac_mac_ctx* ctx, const uint8_t* data, size_t data_length) {
    //printf("MAC add %d: ", data_length); print_hex(data, data_length); printf("\n");
    size_t data_idx = 0;
    //uint8_t* xormac = ctx->xormac;

    if(ctx->idx_in != 0) {
        uint8_t buff_add = 8 - ctx->idx_in;
        if(data_length < buff_add) {
            buff_add = data_length;
        }
        memcpy(ctx->buffer_in + ctx->idx_in, data, buff_add);
        ctx->idx_in = (ctx->idx_in + buff_add) % 8;
        data_idx += buff_add;

        if(ctx->idx_in == 0) { // buffer_in filled
            for(uint8_t j=0; j<8; ++j) {
                ctx->xormac[j] = ctx->mac[j] ^ ctx->buffer_in[j];
            }
            mbedtls_des_crypt_ecb(&ctx->des, ctx->xormac, ctx->mac);

            printf("DES buf: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                ctx->buffer_in[0], ctx->buffer_in[1], ctx->buffer_in[2], ctx->buffer_in[3],
                ctx->buffer_in[4], ctx->buffer_in[5], ctx->buffer_in[6], ctx->buffer_in[7]);

            //printf("DES1: %02X %02X %02X %02X %02X %02X %02X %02X\n",
                //xormac[0], xormac[1], xormac[2], xormac[3],
                //xormac[4], xormac[5], xormac[6], xormac[7]);
        }
    }

    while(true) {
        if(data_idx + 8 > data_length) {
            // Not a full block
            break;
        }
        for(uint8_t j=0; j<8; ++j) {
            ctx->xormac[j] = ctx->mac[j] ^ data[data_idx++];
        }

        mbedtls_des_crypt_ecb(&ctx->des, ctx->xormac, ctx->mac);
        printf("DES add: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
            data[data_idx - 8 + 0], data[data_idx - 8 + 1], data[data_idx - 8 + 2], data[data_idx - 8 + 3],
            data[data_idx - 8 + 4], data[data_idx - 8 + 5], data[data_idx - 8 + 6], data[data_idx - 8 + 7]);

        //printf("DES1: %02X %02X %02X %02X %02X %02X %02X %02X\n",
            //xormac[0], xormac[1], xormac[2], xormac[3],
            //xormac[4], xormac[5], xormac[6], xormac[7]);
    }

    if(data_idx < data_length) {
        ctx->idx_in = data_length - data_idx;
        memcpy(ctx->buffer_in, data + data_idx, ctx->idx_in);
    }

    return true;
}

bool mrtd_bac_mac_pad(mrtd_bac_mac_ctx* ctx) {
    memset(ctx->buffer_in + ctx->idx_in, 0x00, 8 - ctx->idx_in);
    ctx->buffer_in[ctx->idx_in] = 0x80;
    ctx->idx_in = 8;

    mrtd_bac_mac_update(ctx, NULL, 0); // Force processing the buffer_in
    return true;
}

bool mrtd_bac_mac_finalize(mrtd_bac_mac_ctx* ctx, uint8_t output[8]) {
    mrtd_bac_mac_pad(ctx);

    uint8_t tmp[8];
    mbedtls_des_init(&ctx->des);
    mbedtls_des_setkey_dec(&ctx->des, ctx->key+8);
    mbedtls_des_crypt_ecb(&ctx->des, ctx->mac, tmp);

    mbedtls_des_init(&ctx->des);
    mbedtls_des_setkey_enc(&ctx->des, ctx->key);
    mbedtls_des_crypt_ecb(&ctx->des, tmp, output);

    mbedtls_des_free(&ctx->des);
    return true;
}

bool mrtd_bac_mac(const uint8_t* data, size_t data_length, uint8_t* key, uint8_t* output) {
    // MAC
    uint8_t mac[8];
    uint8_t xormac[8];
    uint8_t tmp[8];
    mbedtls_des_context ctx;

    mbedtls_des_init(&ctx);
    mbedtls_des_setkey_enc(&ctx, key);

    memset(mac, 0, 8);
    for(size_t i=0; i<data_length / 8; ++i) {
        for(uint8_t j=0; j<8; ++j) {
            xormac[j] = mac[j] ^ data[i * 8 + j];
        }

        mbedtls_des_crypt_ecb(&ctx, xormac, mac);
        printf("DES1: %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
            xormac[0], xormac[1], xormac[2], xormac[3],
            xormac[4], xormac[5], xormac[6], xormac[7]);
    }

    mbedtls_des_init(&ctx);
    mbedtls_des_setkey_dec(&ctx, key+8);
    mbedtls_des_crypt_ecb(&ctx, mac, tmp);

    mbedtls_des_init(&ctx);
    mbedtls_des_setkey_enc(&ctx, key);
    mbedtls_des_crypt_ecb(&ctx, tmp, output);

    mbedtls_des_free(&ctx);

    return true;
}

bool mrtd_bac_padded_mac(const uint8_t* data, size_t data_length, uint8_t* key, uint8_t* output) {
    //TODO: bufferless padding should be possible with 3DES
    size_t newlength = ((data_length+8)/8)*8; // TODO: return this value too?
    uint8_t padded[newlength]; //TODO: input parameter
    memset(padded, 0, newlength);
    memcpy(padded, data, data_length);
    padded[data_length] = 0x80;

    if(!mrtd_bac_mac(padded, newlength, key, output)) {
        return false;
    }

    return true;
}

size_t mrtd_protect_apdu(uint8_t cla, uint8_t ins, uint8_t p1, uint8_t p2, uint8_t lc, const void* data, int16_t le, uint8_t* key_enc, uint8_t* key_mac, uint64_t ssc, uint8_t* output) {
    //TODO: max size on output?
    size_t idx = 0;

    // CC = MAC( SSC || CmdHeader || DO'87 )
    mrtd_bac_mac_ctx mac_ctx;
    mrtd_bac_mac_init(&mac_ctx, key_mac);
    uint64_t ssc_n = htonll(ssc);
    printf("ssc: %016llx\r\n", ssc);
    //printf("ssc_n: "); print_hex(ssc_n, 8); printf("\n");
    mrtd_bac_mac_update(&mac_ctx, (uint8_t*)&ssc_n, 8);

    // Mask cla
    output[idx++] = cla | 0x0c;
    output[idx++] = ins;
    output[idx++] = p1;
    output[idx++] = p2;

    // Pad Header
    mrtd_bac_mac_update(&mac_ctx, output, idx);
    mrtd_bac_mac_pad(&mac_ctx);

    size_t idx_lc = idx;
    output[idx++] = 0xff; // place holder for Lc

    // Build DO'87
    // TODO: condition on data presence
    // TODO: if ins is odd, use 0x85
    if(lc > 0) {
        size_t newlength = ((lc+8)/8)*8;
        uint8_t padded[newlength];

        output[idx++] = 0x87; // Header
        output[idx++] = newlength + 1;  // Length
        output[idx++] = 0x01; //TODO: check this value

        memset(padded, 0, newlength);
        memcpy(padded, data, lc);
        padded[lc] = 0x80;

        mrtd_bac_encrypt(padded, newlength, key_enc, output + idx);
        idx += newlength;
    }

    // Build DO'97
    if(le >= 0) {
        output[idx++] = 0x97; // Header
        output[idx++] = 0x01; // Length
        output[idx++] = le;
    }

    mrtd_bac_mac_update(&mac_ctx, output + idx_lc + 1, idx - idx_lc - 1);

    // Build DO'8E
    // TODO: conditions?
    {
        output[idx++] = 0x8E; // Header
        output[idx++] = 0x08; // Length

        mrtd_bac_mac_finalize(&mac_ctx, output + idx);
        idx += 8;

        printf("MAC: ");
        for(uint8_t i=0; i<8; ++i) {
            printf("%02X ", output[idx - 8 + i]);
        }
        printf("\r\n");
    }

    output[idx_lc] = idx - idx_lc - 1; // Set Lc

    output[idx++] = 0x00;

    if(le) {
        //TODO: le?
    }

    return idx;
}
