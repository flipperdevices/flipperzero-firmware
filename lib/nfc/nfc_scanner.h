#pragma once

#include <nfc/nfc.h>
#include <nfc/protocols/nfc_protocol.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcScanner NfcScanner;

typedef enum {
    NfcScannerEventTypeDetected,
} NfcScannerEventType;

typedef struct {
    size_t protocol_num;
    NfcProtocol* protocols;
} NfcScannerEventData;

typedef struct {
    NfcScannerEventType type;
    NfcScannerEventData data;
} NfcScannerEvent;

typedef void (*NfcScannerCallback)(NfcScannerEvent event, void* context);

NfcScanner* nfc_scanner_alloc(Nfc* nfc);

void nfc_scanner_free(NfcScanner* instance);

void nfc_scanner_start(NfcScanner* instance, NfcScannerCallback callback, void* context);

void nfc_scanner_stop(NfcScanner* instance);

#ifdef __cplusplus
}
#endif
