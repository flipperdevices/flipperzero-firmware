#pragma once

#include <lib/nfc/protocols/iso14443_4a/iso14443_4a.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MF_DESFIRE_CMD_GET_VERSION (0x60)
#define MF_DESFIRE_CMD_GET_FREE_MEMORY (0x6E)
#define MF_DESFIRE_CMD_GET_KEY_SETTINGS (0x45)
#define MF_DESFIRE_CMD_GET_KEY_VERSION (0x64)
#define MF_DESFIRE_CMD_GET_APPLICATION_IDS (0x6A)
#define MF_DESFIRE_CMD_SELECT_APPLICATION (0x5A)
#define MF_DESFIRE_CMD_GET_FILE_IDS (0x6F)
#define MF_DESFIRE_CMD_GET_FILE_SETTINGS (0xF5)

#define MF_DESFIRE_CMD_READ_DATA (0xBD)
#define MF_DESFIRE_CMD_GET_VALUE (0x6C)
#define MF_DESFIRE_CMD_READ_RECORDS (0xBB)

#define MF_DESFIRE_FLAG_HAS_NEXT (0xAF)

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
    uint32_t bytes_free;
    bool is_present;
} MfDesfireFreeMemory; // EV1+ only

typedef struct MfDesfireKeyVersion {
    uint8_t id;
    uint8_t version;
    struct MfDesfireKeyVersion* next;
} MfDesfireKeyVersion;

typedef struct {
    struct {
        bool is_config_changeable : 1;
        bool is_free_directory_list : 1;
        bool is_free_create_delete : 1;
        bool is_master_key_changeable : 1;
        uint8_t change_key_id : 4;
        uint8_t max_keys : 4;
        uint8_t flags : 4;
    } settings;
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
    MfDesfireErrorNotPresent,
    MfDesfireErrorProtocol,
    MfDesfireErrorTimeout,
} MfDesfireError;

typedef struct {
    Iso14443_4aData iso14443_4a_data;
    MfDesfireVersion version;
    MfDesfireFreeMemory free_memory;
    MfDesfireKeySettings master_key_settings;
    MfDesfireApplication* app_head;
} MfDesfireData;

bool mf_desfire_detect_protocol(NfcaData* nfca_data);

#ifdef __cplusplus
}
#endif
