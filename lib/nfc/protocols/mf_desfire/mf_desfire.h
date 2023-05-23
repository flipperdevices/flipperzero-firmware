#pragma once

#include <lib/nfc/protocols/nfca/nfca.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t hw_vendor;
    uint8_t hw_type;
    uint8_t hw_subtype;
    uint8_t hw_major;
    uint8_t hw_minor;
    uint8_t hw_storage;
    uint8_t hw_proto;

    uint8_t sw_vendor;
    uint8_t sw_type;
    uint8_t sw_subtype;
    uint8_t sw_major;
    uint8_t sw_minor;
    uint8_t sw_storage;
    uint8_t sw_proto;

    uint8_t uid[7];
    uint8_t batch[5];
    uint8_t prod_week;
    uint8_t prod_year;
} MfDesfireVersion;

typedef struct {
    uint32_t bytes;
} MfDesfireFreeMemory; // EV1+ only

typedef struct MfDesfireKeyVersion {
    uint8_t id;
    uint8_t version;
    struct MfDesfireKeyVersion* next;
} MfDesfireKeyVersion;

typedef struct {
    uint8_t change_key_id;
    bool config_changeable;
    bool free_create_delete;
    bool free_directory_list;
    bool master_key_changeable;
    uint8_t flags;
    uint8_t max_keys;
    MfDesfireKeyVersion* key_version_head;
} MfDesfireKeySettings;

typedef enum {
    MfDesfireFileTypeStandard = 0,
    MfDesfireFileTypeBackup = 1,
    MfDesfireFileTypeValue = 2,
    MfDesfireFileTypeLinearRecord = 3,
    MfDesfireFileTypeCyclicRecord = 4,
} MfDesfireFileType;

typedef enum {
    MfDesfireFileCommunicationSettingsPlaintext = 0,
    MfDesfireFileCommunicationSettingsAuthenticated = 1,
    MfDesfireFileCommunicationSettingsEnciphered = 3,
} MfDesfireFileCommunicationSettings;

typedef struct MifareDesfireFile {
    uint8_t id;
    MfDesfireFileType type;
    MfDesfireFileCommunicationSettings comm;
    uint16_t access_rights;
    union {
        struct {
            uint32_t size;
        } data;
        struct {
            uint32_t lo_limit;
            uint32_t hi_limit;
            uint32_t limited_credit_value;
            bool limited_credit_enabled;
        } value;
        struct {
            uint32_t size;
            uint32_t max;
            uint32_t cur;
        } record;
    } settings;
    uint8_t* contents;

    struct MifareDesfireFile* next;
} MfDesfireFile;

typedef struct MfDesfireApplication {
    uint8_t id[3];
    MfDesfireKeySettings* key_settings;
    MfDesfireFile* file_head;

    struct MfDesfireApplication* next;
} MfDesfireApplication;

typedef enum {
    MfDesfireErrorNone,
} MfDesfireError;

typedef struct {
    NfcaData nfca_data;
    MfDesfireVersion version;
    MfDesfireFreeMemory* free_memory;
    MfDesfireKeySettings* master_key_settings;
    MfDesfireApplication* app_head;
} MfDesfireData;

bool mf_desfire_detect_protocol(NfcaData* nfca_data);

#ifdef __cplusplus
}
#endif
