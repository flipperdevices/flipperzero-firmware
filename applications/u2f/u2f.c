#include <furi.h>
#include "u2f_hid.h"
#include <furi-hal.h>

#include <furi-hal-console.h>
#include <furi-hal-trng.h>

#include "toolbox/sha256.h"
#include "toolbox/hmac_sha256.h"
#include "micro-ecc/uECC.h"

#define TAG "U2F"
#define WORKER_TAG TAG "Worker"

#define U2F_CMD_REGISTER 0x01
#define U2F_CMD_AUTHENTICATE 0x02
#define U2F_CMD_VERSION 0x03

typedef struct {
    uint8_t format;
    uint8_t xy[64];
} __attribute__((packed)) U2fPubKey;

typedef struct {
    uint8_t len;
    uint8_t hash[32];
    uint8_t nonce[32];
} __attribute__((packed)) U2fKeyHandle;

typedef struct {
    uint8_t cla;
    uint8_t ins;
    uint8_t p1;
    uint8_t p2;
    uint8_t len[3];
    uint8_t challenge[32];
    uint8_t app_id[32];
} __attribute__((packed)) U2fRegisterReq;

typedef struct {
    uint8_t reserved;
    U2fPubKey pub_key;
    U2fKeyHandle key_handle;
    uint8_t cert[];
} __attribute__((packed)) U2fRegisterResp;

typedef struct {
    uint8_t cla;
    uint8_t ins;
    uint8_t p1;
    uint8_t p2;
    uint8_t len[3];
    uint8_t challenge[32];
    uint8_t app_id[32];
    U2fKeyHandle key_handle;
} __attribute__((packed)) U2fAuthReq;

typedef struct {
    uint8_t user_present;
    uint32_t counter;
    uint8_t signature[];
} __attribute__((packed)) U2fAuthResp;

static const uint8_t ver_str[] = {"U2F_V2"};

static const uint8_t state_no_error[] = {0x90, 0x00};
static const uint8_t state_not_supported[] = {0x6D, 0x00};
static const uint8_t state_user_missing[] = {0x69, 0x85};
static const uint8_t state_wrong_data[] = {0x6A, 0x80};

static const uint8_t cert_der[] = {
    0x30, 0x82, 0x01, 0x5b, 0x30, 0x82, 0x01, 0x01, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x14, 0x1c,
    0x06, 0xf2, 0x12, 0x13, 0x92, 0x87, 0x53, 0x62, 0xbd, 0x72, 0xf9, 0xc3, 0x25, 0x0f, 0x32, 0xf1,
    0xd1, 0x1b, 0xd2, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x30,
    0x21, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55, 0x53, 0x31, 0x12,
    0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x09, 0x55, 0x32, 0x46, 0x20, 0x54, 0x6f, 0x6b,
    0x65, 0x6e, 0x30, 0x1e, 0x17, 0x0d, 0x32, 0x31, 0x31, 0x32, 0x30, 0x37, 0x31, 0x36, 0x33, 0x32,
    0x34, 0x30, 0x5a, 0x17, 0x0d, 0x33, 0x31, 0x31, 0x32, 0x30, 0x35, 0x31, 0x36, 0x33, 0x32, 0x34,
    0x30, 0x5a, 0x30, 0x21, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x55,
    0x53, 0x31, 0x12, 0x30, 0x10, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x09, 0x55, 0x32, 0x46, 0x20,
    0x54, 0x6f, 0x6b, 0x65, 0x6e, 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d,
    0x02, 0x01, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04,
    0x42, 0x06, 0x15, 0x9d, 0xab, 0xf5, 0xd2, 0x07, 0xa6, 0xfc, 0x29, 0xb6, 0x6d, 0x92, 0x1c, 0x5f,
    0x76, 0x5e, 0xa3, 0x13, 0x6e, 0xf2, 0xc5, 0x8b, 0xe1, 0x21, 0x7a, 0xc0, 0xfa, 0x64, 0xc9, 0x42,
    0x0f, 0x5c, 0x45, 0xaa, 0xea, 0xfc, 0x51, 0xf0, 0x10, 0xf7, 0x0b, 0x05, 0x9d, 0x4c, 0x59, 0x22,
    0x80, 0xab, 0x0c, 0x56, 0x35, 0x89, 0x35, 0x7c, 0x3f, 0xf4, 0x58, 0xd5, 0xf5, 0xe1, 0x9f, 0x00,
    0xa3, 0x17, 0x30, 0x15, 0x30, 0x13, 0x06, 0x0b, 0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0xe5, 0x1c,
    0x02, 0x01, 0x01, 0x04, 0x04, 0x03, 0x02, 0x05, 0x20, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48,
    0xce, 0x3d, 0x04, 0x03, 0x02, 0x03, 0x48, 0x00, 0x30, 0x45, 0x02, 0x20, 0x15, 0x37, 0x5d, 0xc4,
    0x7c, 0x39, 0xa6, 0x05, 0x9e, 0xda, 0xfb, 0xf4, 0x3c, 0x60, 0x97, 0x47, 0xa4, 0xb3, 0x9b, 0x19,
    0x80, 0x75, 0x0f, 0xc7, 0x83, 0x30, 0xad, 0x19, 0x85, 0x01, 0xe5, 0x37, 0x02, 0x21, 0x00, 0xc1,
    0x21, 0x1f, 0x14, 0x2a, 0x56, 0x3a, 0x76, 0x71, 0xa8, 0x48, 0x10, 0x10, 0xa4, 0x0d, 0x9a, 0xff,
    0x90, 0xe0, 0x3b, 0x06, 0xa0, 0x3e, 0x18, 0xf3, 0x2c, 0x62, 0xd1, 0x27, 0x86, 0x5d, 0x5a};

static const uint8_t cert_key[32] = {
    0xa1, 0xb4, 0x4b, 0xec, 0xab, 0x8e, 0x87, 0x18, 0xfd, 0x21, 0x30, 0xc8, 0x50, 0xf3, 0x94, 0x97,
    0xb5, 0xe6, 0x9a, 0x75, 0xd7, 0x24, 0x16, 0x61, 0x95, 0x6c, 0xde, 0x60, 0x1f, 0xe1, 0xb6, 0xc8};

static uint8_t device_key[32];
static uint32_t counter;

void u2f_dump(char* tag, uint8_t* buf, uint32_t len) {
    furi_hal_console_printf("%s [%u]:", tag, len);
    for(uint16_t i = 0; i < len; i++) {
        if(i % 32 == 0) furi_hal_console_printf("\r\n");
        furi_hal_console_printf("%02X", buf[i]);
    }
    furi_hal_console_printf("\r\n");
}

void u2f_init() {
    furi_hal_trng_fill_buf(device_key, 32);
    u2f_dump("Device key", device_key, 32);
    counter = 0;
}

static int u2f_uecc_random(uint8_t* dest, unsigned size) {
    furi_hal_trng_fill_buf(dest, size);
    return 1;
}

static bool u2f_check_user_present() {
    //TODO
    return true;
}

static uint8_t u2f_der_encode_int(uint8_t* der, uint8_t* val, uint8_t val_len) {
    der[0] = 0x02; // Integer

    uint8_t len = 2;
    // Omit leading zeros
    while(val[0] == 0 && val_len > 0) {
        ++val;
        --val_len;
    }

    // Check if integer is negative
    if(val[0] > 0x7f) der[len++] = 0;

    memcpy(der + len, val, val_len);
    len += val_len;

    der[1] = len - 2;
    return len;
}

static uint8_t u2f_der_encode_signature(uint8_t* der, uint8_t* sig) {
    der[0] = 0x30;

    uint8_t len = 2;
    len += u2f_der_encode_int(der + len, sig, 32);
    len += u2f_der_encode_int(der + len, sig + 32, 32);

    der[1] = len - 2;
    return len;
}

uint16_t u2f_register(uint8_t* buf) {
    U2fRegisterReq* req = (U2fRegisterReq*)buf;
    U2fRegisterResp* resp = (U2fRegisterResp*)buf;
    U2fKeyHandle handle;
    uint8_t private[32];
    U2fPubKey pub_key;
    uint8_t hash[32];
    uint8_t signature[64];

    if(u2f_check_user_present() == false) {
        memcpy(&buf[0], state_user_missing, 2);
        return 2;
    }

    hmac_sha256_context hmac_ctx;
    sha256_context sha_ctx;
    const struct uECC_Curve_t* p_curve = uECC_secp256r1();
    uECC_set_rng(u2f_uecc_random);

    u2f_dump("Challenge:", req->challenge, 32);
    u2f_dump("AppID:", req->app_id, 32);

    handle.len = 32 * 2;
    // Generate random nonce
    furi_hal_trng_fill_buf(handle.nonce, 32);
    u2f_dump("Nonce", handle.nonce, 32);

    // Generate private key
    hmac_sha256_init(&hmac_ctx, device_key);
    hmac_sha256_update(&hmac_ctx, req->app_id, 32);
    hmac_sha256_update(&hmac_ctx, handle.nonce, 32);
    hmac_sha256_finish(&hmac_ctx, device_key, private);
    u2f_dump("Private key", private, 32);

    // Generate private key handle
    hmac_sha256_init(&hmac_ctx, device_key);
    hmac_sha256_update(&hmac_ctx, private, 32);
    hmac_sha256_update(&hmac_ctx, req->app_id, 32);
    hmac_sha256_finish(&hmac_ctx, device_key, handle.hash);
    u2f_dump("Key handle", handle.hash, 32 * 2);

    // Generate public key
    pub_key.format = 0x04; // Uncompressed point
    uECC_compute_public_key(private, pub_key.xy, p_curve);
    u2f_dump("Public key", pub_key.xy, 64);

    // Generate signature
    uint8_t reserved_byte = 0;
    sha256_start(&sha_ctx);
    sha256_update(&sha_ctx, &reserved_byte, 1);
    sha256_update(&sha_ctx, req->app_id, 32);
    sha256_update(&sha_ctx, req->challenge, 32);
    sha256_update(&sha_ctx, handle.hash, handle.len);
    sha256_update(&sha_ctx, (uint8_t*)&pub_key, 65);
    sha256_finish(&sha_ctx, hash);
    u2f_dump("Hash", hash, 32);

    uECC_sign(cert_key, hash, 32, signature, p_curve);
    u2f_dump("Signature", signature, 64);

    // Encode response message
    resp->reserved = 0x05;
    memcpy(&(resp->pub_key), &pub_key, sizeof(U2fPubKey));
    memcpy(&(resp->key_handle), &handle, sizeof(U2fKeyHandle));
    memcpy(resp->cert, cert_der, sizeof(cert_der));
    uint8_t signature_len = u2f_der_encode_signature(resp->cert + sizeof(cert_der), signature);
    u2f_dump("Signature encoded", resp->cert + sizeof(cert_der), signature_len);
    memcpy(resp->cert + sizeof(cert_der) + signature_len, state_no_error, 2);

    return (sizeof(U2fRegisterResp) + sizeof(cert_der) + signature_len + 2);
}

uint16_t u2f_authenticate(uint8_t* buf) {
    U2fAuthReq* req = (U2fAuthReq*)buf;
    U2fAuthResp* resp = (U2fAuthResp*)buf;
    uint8_t priv_key[32];
    uint8_t mac_control[32];
    hmac_sha256_context hmac_ctx;
    sha256_context sha_ctx;
    uint8_t flags = 1;
    uint8_t hash[32];
    uint8_t signature[64];

    u2f_dump("Control:", &req->p1, 1);
    u2f_dump("Challenge:", req->challenge, 32);
    u2f_dump("AppID:", req->app_id, 32);
    u2f_dump("Handle:", req->key_handle.hash, 64);

    bool user_present = u2f_check_user_present();
    if(user_present == false) {
        memcpy(&buf[0], state_user_missing, 2);
        return 2;
    }

    // Generate hash
    sha256_start(&sha_ctx);
    sha256_update(&sha_ctx, req->app_id, 32);
    sha256_update(&sha_ctx, &flags, 1);
    sha256_update(&sha_ctx, (uint8_t*)&counter, 4);
    sha256_update(&sha_ctx, req->challenge, 32);
    sha256_finish(&sha_ctx, hash);
    u2f_dump("Hash", hash, 32);

    // Recover private key
    hmac_sha256_init(&hmac_ctx, device_key);
    hmac_sha256_update(&hmac_ctx, req->app_id, 32);
    hmac_sha256_update(&hmac_ctx, req->key_handle.nonce, 32);
    hmac_sha256_finish(&hmac_ctx, device_key, priv_key);
    u2f_dump("Private key", priv_key, 32);

    // Generate and verify private key handle
    hmac_sha256_init(&hmac_ctx, device_key);
    hmac_sha256_update(&hmac_ctx, priv_key, 32);
    hmac_sha256_update(&hmac_ctx, req->app_id, 32);
    hmac_sha256_finish(&hmac_ctx, device_key, mac_control);
    u2f_dump("MAC", mac_control, 32);

    if(memcmp(req->key_handle.hash, mac_control, 32) != 0) {
        FURI_LOG_I(TAG, "Wrong handle!");
        memcpy(&buf[0], state_wrong_data, 2);
        return 2;
    }

    if(req->p1 == 0x07) { // Check-only: don't need to send full request
        memcpy(&buf[0], state_user_missing, 2);
        return 2;
    }

    const struct uECC_Curve_t* p_curve = uECC_secp256r1();
    uECC_set_rng(u2f_uecc_random);

    uECC_sign(priv_key, hash, 32, signature, p_curve);
    u2f_dump("Signature", signature, 64);

    resp->user_present = flags;
    resp->counter = counter;
    uint8_t signature_len = u2f_der_encode_signature(resp->signature, signature);
    u2f_dump("Signature encoded", resp->signature, signature_len);
    memcpy(resp->signature + signature_len, state_no_error, 2);

    counter++;

    return (sizeof(U2fAuthResp) + signature_len + 2);
}

void u2f_test() {
    uint8_t data[64];
    uint8_t key[32];
    uint8_t hash[32];
    uint8_t hmac_res[64];

    furi_hal_trng_fill_buf(data, 64);
    u2f_dump("Data:", data, 64);

    furi_hal_trng_fill_buf(key, 32);
    u2f_dump("Key:", key, 32);

    sha256(data, 64, hash);
    u2f_dump("Hash:", hash, 32);

    hmac_sha256_context hmac;
    hmac_sha256_init(&hmac, key);
    hmac_sha256_update(&hmac, data, 64);
    hmac_sha256_finish(&hmac, key, hmac_res);
    u2f_dump("HMAC:", hash, 64);

    u2f_dump("HMAC:", (uint8_t*)cert_key, 32);
}

uint16_t u2f_msg_parse(uint8_t* buf, uint16_t len) {
    if((buf[0] != 0x00) && (len < 5)) return 0;
    if(buf[1] == U2F_CMD_REGISTER) { // Register request
        return u2f_register(buf);

    } else if(buf[1] == U2F_CMD_AUTHENTICATE) { // Authenticate request
        return u2f_authenticate(buf);

    } else if(buf[1] == U2F_CMD_VERSION) { // Get U2F version string
        memcpy(&buf[0], ver_str, 6);
        memcpy(&buf[6], state_no_error, 2);
        return 8;
    } else {
        memcpy(&buf[0], state_not_supported, 2);
        return 2;
    }
    return 0;
}
