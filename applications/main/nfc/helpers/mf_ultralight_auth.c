#include "mf_ultralight_auth.h"

#include <furi.h>
#include <mbedtls/sha1.h>

MfUltralightAuth* mf_ultralight_auth_alloc() {
    MfUltralightAuth* instance = malloc(sizeof(MfUltralightAuth));

    return instance;
}

void mf_ultralight_auth_free(MfUltralightAuth* instance) {
    furi_assert(instance);

    free(instance);
}

void mf_ultralight_auth_reset(MfUltralightAuth* instance) {
    furi_assert(instance);

    instance->type = MfUltralightAuthTypeNone;
    memset(&instance->password, 0, sizeof(MfUltralightAuthPassword));
    memset(&instance->pack, 0, sizeof(MfUltralightAuthPack));
}

bool mf_ultralight_generate_amiibo_pass(MfUltralightAuth* instance, uint8_t* uid, uint16_t uid_len) {
    furi_assert(instance);
    furi_assert(uid);

    uint32_t pwd = 0;
    bool generated = false;
    if(uid_len == 7) {
        pwd |= (uid[1] ^ uid[3] ^ 0xAA) << 24;
        pwd |= (uid[2] ^ uid[4] ^ 0x55) << 16;
        pwd |= (uid[3] ^ uid[5] ^ 0xAA) << 8;
        pwd |= uid[4] ^ uid[6] ^ 0x55;
        instance->password.pass = pwd;
        generated = true;
    }

    return generated;
}

bool mf_ultralight_generate_xiaomi_pass(MfUltralightAuth* instance, uint8_t* uid, uint16_t uid_len) {
    furi_assert(instance);
    furi_assert(uid);

    uint32_t pwd = 0;
    uint8_t hash[20];
    bool generated = false;
    if(uid_len == 7) {
        mbedtls_sha1(uid, uid_len, hash);
        pwd |= (hash[hash[0] % 20]) << 24;
        pwd |= (hash[(hash[0] + 5) % 20]) << 16;
        pwd |= (hash[(hash[0] + 13) % 20]) << 8;
        pwd |= (hash[(hash[0] + 17) % 20]);
        instance->password.pass = pwd;
        generated = true;
    }

    return generated;
}