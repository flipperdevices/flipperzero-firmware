#pragma once

#include <nfc/protocols/iso15693_3/iso15693_3.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SLIX_PASSWORD_SIZE (4U)
#define SLIX_SIGNATURE_SIZE (32U)

typedef enum {
    SlixErrorNone,
    SlixErrorTimeout,
    SlixErrorFormat,
    SlixErrorNotSupported,
    SlixErrorInternal,
    SlixErrorUnknown,
} SlixError;

typedef enum {
    SlixTypeSlix,
    SlixTypeSlixS,
    SlixTypeSlixL,
    SlixTypeSlix2,
    SlixTypeNum,
} SlixType;

typedef struct {
    bool is_present;
    uint8_t data[SLIX_PASSWORD_SIZE];
} SlixPassword;

typedef struct {
    SlixPassword read;
    SlixPassword write;
    SlixPassword privacy;
    SlixPassword destroy;
    SlixPassword eas;
} SlixPasswords;

typedef struct {
    bool is_present;
    bool mode;
} SlixPrivacy;

typedef struct {
    bool is_present;
    uint8_t data[SLIX_SIGNATURE_SIZE];
} SlixSignature;

typedef struct {
    bool is_present;
    uint8_t pointer;
    uint8_t condition;
} SlixProtection;

typedef struct {
    struct {
        uint8_t pointer;
        uint8_t condition;
    } protection;
    uint8_t unknown_byte_0;
    uint8_t unknown_byte_1;
    uint8_t unknown_byte_2;
    uint8_t unknown_byte_3;
    uint8_t unknown_byte_4;
} SlixSystemInfo;

typedef struct {
    Iso15693_3Data* iso15693_3_data;
    SlixSystemInfo system_info;
    SlixSignature signature;
    SlixPasswords passwords;
    SlixPrivacy privacy_mode;
    SlixProtection protection;
} SlixData;

SlixData* slix_alloc();

void slix_free(SlixData* data);

void slix_reset(SlixData* data);

void slix_copy(SlixData* data, const SlixData* other);

bool slix_verify(SlixData* data, const FuriString* device_type);

bool slix_load(SlixData* data, FlipperFormat* ff, uint32_t version);

bool slix_save(const SlixData* data, FlipperFormat* ff);

bool slix_is_equal(const SlixData* data, const SlixData* other);

const char* slix_get_device_name(const SlixData* data, NfcDeviceNameType name_type);

const uint8_t* slix_get_uid(const SlixData* data, size_t* uid_len);

bool slix_set_uid(SlixData* data, const uint8_t* uid, size_t uid_len);

const Iso15693_3Data* slix_get_base_data(const SlixData* data);

// Getters and tests

SlixType slix_get_type(const SlixData* data);

#ifdef __cplusplus
}
#endif
