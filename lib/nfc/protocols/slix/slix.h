#pragma once

#include <nfc/protocols/iso15693_3/iso15693_3.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SLIX_PASSWORD_SIZE (4U)
#define SLIX_SIGNATURE_SIZE (32U)

#define SLIX_CMD_GET_NXP_SYSINFO (0xABU)
#define SLIX_CMD_READ_SIGNATURE (0xBDU)

typedef enum {
    SlixErrorNone,
    SlixErrorFormat,
    SlixErrorNotSupported,
    SlixErrorWrongManufacturer,
    SlixErrorInternal,
    SlixErrorUnknown,
} SlixError;

typedef enum {
    SlixTypeSlix,
    SlixTypeSlixS,
    SlixTypeSlixL,
    SlixTypeSlix2,
    SlixTypeMax,
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
    Iso15693_3Data* iso15693_3_data;
    SlixType type;
    SlixPasswords passwords;
    SlixSignature signature;
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

#ifdef __cplusplus
}
#endif
