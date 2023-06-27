#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>

#include <furi_hal_nfc.h>
#include <lib/nfc/deprecated/helpers/mf_classic_dict.h>
#include <lib/nfc/deprecated/protocols/emv.h>
#include <lib/nfc/deprecated/protocols/mifare_ultralight.h>
#include <lib/nfc/deprecated/protocols/mifare_classic.h>
#include <lib/nfc/deprecated/protocols/mifare_desfire.h>
#include <lib/nfc/deprecated/protocols/nfcv.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NFC_DEV_NAME_MAX_LEN 22
#define NFC_READER_DATA_MAX_SIZE 64
#define NFC_DICT_KEY_BATCH_SIZE 10

#define NFC_APP_EXTENSION ".nfc"
#define NFC_APP_SHADOW_EXTENSION ".shd"

typedef void (*NfcLoadingCallback)(void* context, bool state);

typedef enum {
    NfcDeviceOldProtocolUnknown,
    NfcDeviceOldProtocolEMV,
    NfcDeviceOldProtocolMifareUl,
    NfcDeviceOldProtocolMifareClassic,
    NfcDeviceOldProtocolMifareDesfire,
    NfcDeviceOldProtocolNfcV
} NfcProtocolOld;

typedef enum {
    NfcDeviceOldSaveFormatUid,
    NfcDeviceOldSaveFormatBankCard,
    NfcDeviceOldSaveFormatMifareUl,
    NfcDeviceOldSaveFormatMifareClassic,
    NfcDeviceOldSaveFormatMifareDesfire,
    NfcDeviceOldSaveFormatNfcV,
} NfcDeviceOldSaveFormat;

typedef struct {
    uint8_t data[NFC_READER_DATA_MAX_SIZE];
    uint16_t size;
} NfcReaderRequestData;

typedef struct {
    MfClassicDict* dict;
    uint8_t current_sector;
} NfcMfClassicDictAttackData;

typedef enum {
    NfcReadModeAuto,
    NfcReadModeMfClassic,
    NfcReadModeMfUltralight,
    NfcReadModeMfDesfire,
    NfcReadModeNFCA,
} NfcReadMode;

typedef struct {
    FuriHalNfcDevData nfc_data;
    NfcProtocolOld protocol;
    NfcReadMode read_mode;
    union {
        NfcReaderRequestData reader_data;
        NfcMfClassicDictAttackData mf_classic_dict_attack_data;
        MfUltralightAuth mf_ul_auth;
    };
    union {
        EmvData emv_data;
        MfUltralightData mf_ul_data;
        MfClassicData mf_classic_data;
        MifareDesfireData mf_df_data;
        NfcVData nfcv_data;
    };
    FuriString* parsed_data;
} NfcDeviceOldDataOld;

typedef struct {
    Storage* storage;
    DialogsApp* dialogs;
    NfcDeviceOldDataOld dev_data;
    char dev_name[NFC_DEV_NAME_MAX_LEN + 1];
    FuriString* load_path;
    FuriString* folder;
    NfcDeviceOldSaveFormat format;
    bool shadow_file_exist;

    NfcLoadingCallback loading_cb;
    void* loading_cb_ctx;
} NfcDeviceOld;

NfcDeviceOld* nfc_device_old_alloc();

void nfc_device_old_free(NfcDeviceOld* nfc_dev);

void nfc_device_old_set_name(NfcDeviceOld* dev, const char* name);

bool nfc_device_old_save(NfcDeviceOld* dev, const char* dev_name);

bool nfc_device_old_save_shadow(NfcDeviceOld* dev, const char* dev_name);

bool nfc_device_old_load(NfcDeviceOld* dev, const char* file_path, bool show_dialog);

bool nfc_device_old_load_key_cache(NfcDeviceOld* dev);

bool nfc_file_select(NfcDeviceOld* dev);

void nfc_device_old_data_clear(NfcDeviceOldDataOld* dev);

void nfc_device_old_clear(NfcDeviceOld* dev);

bool nfc_device_old_delete(NfcDeviceOld* dev, bool use_load_path);

bool nfc_device_old_restore(NfcDeviceOld* dev, bool use_load_path);

void nfc_device_old_set_loading_callback(
    NfcDeviceOld* dev,
    NfcLoadingCallback callback,
    void* context);

#ifdef __cplusplus
}
#endif
