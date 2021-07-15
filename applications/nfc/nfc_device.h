#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "mifare_ultralight.h"

#define NFC_DEV_NAME_MAX_LEN 22
#define NFC_FILE_NAME_MAX_LEN 120

typedef enum {
    NfcDeviceNfca,
    NfcDeviceNfcb,
    NfcDeviceNfcf,
    NfcDeviceNfcv,
} NfcDeviceType;

typedef enum {
    NfcDeviceProtocolUnknown,
    NfcDeviceProtocolEMV,
    NfcDeviceProtocolMfUltralight,
} NfcProtocol;

typedef struct {
    uint8_t uid_len;
    uint8_t uid[10];
    uint8_t atqa[2];
    uint8_t sak;
    NfcDeviceType device;
    NfcProtocol protocol;
} NfcDeviceCommomData;

typedef struct {
    char name[32];
    uint8_t number[8];
} NfcEmvData;

typedef struct {
    NfcDeviceCommomData nfc_data;
    union {
        NfcEmvData emv_data;
        MifareUlData mf_ul_data;
    };
} NfcDeviceData;

typedef struct {
    NfcDeviceData dev_data;
    char dev_name[NFC_DEV_NAME_MAX_LEN];
    char file_name[NFC_FILE_NAME_MAX_LEN];
} NfcDevice;

void nfc_device_set_name(NfcDevice* dev, const char* name);

bool nfc_device_save(NfcDevice* dev, const char* dev_name);

bool nfc_device_load(NfcDevice* dev, const char* file_path);

bool nfc_file_select(NfcDevice* dev);
