#pragma once

#include <rfal_nfc.h>
#include <st_errno.h>

static inline const char* nfc_get_nfca_type(rfalNfcaListenDeviceType type) {
    if (type==RFAL_NFCA_T1T) {
        return "T1T";
    } else if (type==RFAL_NFCA_T2T) {
        return "T2T";
    } else if (type==RFAL_NFCA_T4T) {
        return "T4T";
    } else if (type==RFAL_NFCA_NFCDEP) {
        return "NFCDEP";
    } else if (type==RFAL_NFCA_T4T_NFCDEP) {
        return "T4T_NFCDEP";
    } else {
        return "Unknown";
    }
}

typedef enum {
    NfcDeviceTypeNfca,
    NfcDeviceTypeNfcb,
    NfcDeviceTypeNfcf,
    NfcDeviceTypeNfcv,
    NfcDeviceTypeNfcMifare
} NfcDeviceType;

typedef struct {
    NfcDeviceType type;
    union {
        rfalNfcaListenDevice nfca;
        rfalNfcbListenDevice nfcb;
        rfalNfcfListenDevice nfcf;
        rfalNfcvListenDevice nfcv;
    };
} NfcDevice;

typedef enum {
    NfcWorkerStateNone,
    NfcWorkerStateBroken,
    NfcWorkerStateReady,
    NfcWorkerStatePolling,
} NfcWorkerState;

typedef enum {
    NfcMessageTypeRead,
    NfcMessageTypeFieldOn,
    NfcMessageTypeFieldOff,
    NfcMessageTypeDeviceFound,
    NfcMessageTypeWorkerStateChange,
} NfcMessageType;

typedef struct {
    NfcMessageType type;
    union {
        NfcDevice device;
        NfcWorkerState worker_state;
    };
} NfcMessage;