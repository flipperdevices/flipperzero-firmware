#pragma once

#include <nfc/nfc.h>
#include "protocols/nfc_magic_protocols.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcMagicScannerEventTypeDetected,
    NfcMagicScannerEventTypeDetectedNotMagic,
    NfcMagicScannerEventTypeNotDetected,
} NfcMagicScannerEventType;

typedef struct {
    NfcMagicProtocol protocol;
} NfcMagicScannerEventData;

typedef struct {
    NfcMagicScannerEventType type;
    NfcMagicScannerEventData data;
} NfcMagicScannerEvent;

typedef void (*NfcMagicScannerCallback)(NfcMagicScannerEvent event, void* context);

typedef struct NfcMagicScanner NfcMagicScanner;

NfcMagicScanner* nfc_magic_scanner_alloc(Nfc* nfc);

void nfc_magic_scanner_free(NfcMagicScanner* instance);

void nfc_magic_scanner_set_gen4_password(NfcMagicScanner* instance, uint32_t password);

void nfc_magic_scanner_start(
    NfcMagicScanner* instance,
    NfcMagicScannerCallback callback,
    void* context);

void nfc_magic_scanner_stop(NfcMagicScanner* instance);

#ifdef __cplusplus
}
#endif
