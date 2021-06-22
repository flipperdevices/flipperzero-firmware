#pragma once

#include <rfal_nfc.h>
#include <st_errno.h>

#include <gui/view_dispatcher.h>
#include "nfc_worker.h"

typedef struct {
    NfcWorker* worker;
    ViewDispatcher* view_dispatcher;
    osMessageQueueId_t message_queue;
} NfcCommon;

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
} NfcDeviceData;

typedef struct {
    bool found;
    NfcDeviceData data;
} NfcDetectModel;

typedef struct {
    char name[32];
    uint8_t number[8];
} NfcEmvData;

typedef struct {
    bool found;
    NfcDeviceData nfc_data;
    NfcEmvData emv_data;
} NfcEmvModel;

typedef struct {
    uint8_t uid[7];
    uint8_t man_block[12];
    uint8_t otp[4];
} NfcMfUlData;

typedef struct {
    bool found;
    NfcDeviceData nfc_data;
    NfcMfUlData nfc_mf_ul_data;
} NfcMifareUlModel;

typedef enum {
    NfcEventDetect,
    NfcEventEmv,
    NfcEventMifareUl,
} NfcEvent;

typedef union {
    NfcDetectModel nfc_detect_model;
    NfcEmvModel nfc_emv;
    NfcMifareUlModel nfc_mifare_ul;
} NfcMessage;

static inline const char* nfc_get_dev_type(rfalNfcDevType type) {
    if(type == RFAL_NFC_LISTEN_TYPE_NFCA) {
        return "NFC-A";
    } else if(type == RFAL_NFC_LISTEN_TYPE_NFCB) {
        return "NFC-B";
    } else if(type == RFAL_NFC_LISTEN_TYPE_NFCF) {
        return "NFC-F";
    } else if(type == RFAL_NFC_LISTEN_TYPE_NFCB) {
        return "NFC-B";
    } else if(type == RFAL_NFC_LISTEN_TYPE_NFCV) {
        return "NFC-V";
    } else if(type == RFAL_NFC_LISTEN_TYPE_ST25TB) {
        return "NFC-ST25TB";
    } else if(type == RFAL_NFC_LISTEN_TYPE_AP2P) {
        return "NFC-AP2P";
    } else {
        return "Unknown";
    }
}

static inline const char* nfc_get_nfca_type(rfalNfcaListenDeviceType type) {
    if(type == RFAL_NFCA_T1T) {
        return "T1T";
    } else if(type == RFAL_NFCA_T2T) {
        return "T2T";
    } else if(type == RFAL_NFCA_T4T) {
        return "T4T";
    } else if(type == RFAL_NFCA_NFCDEP) {
        return "NFCDEP";
    } else if(type == RFAL_NFCA_T4T_NFCDEP) {
        return "T4T_NFCDEP";
    } else {
        return "Unknown";
    }
}

static inline const char* nfc_get_protocol(NfcProtocol protocol) {
    if(protocol == NfcDeviceProtocolEMV) {
        return "EMV";
    } else if(protocol == NfcDeviceProtocolMfUltralight) {
        return "Mifare UL";
    } else {
        return "Unknown";
    }
}

typedef enum {
    // From Menu
    NfcMessageTypeDetect,
    NfcMessageTypeEmulate,
    NfcMessageTypeEMV,
    NfcMessageTypeField,
    NfcMessageTypeReadMfUltralight,
    NfcMessageTypeStop,
    NfcMessageTypeExit,
    // From Worker
    NfcMessageTypeDeviceFound,
    NfcMessageTypeDeviceNotFound,
    NfcMessageTypeEMVFound,
    NfcMessageTypeEMVNotFound,
    NfcMessageTypeMfUlFound,
    NfcMessageTypeMfUlNotFound,
} NfcMessageType;
