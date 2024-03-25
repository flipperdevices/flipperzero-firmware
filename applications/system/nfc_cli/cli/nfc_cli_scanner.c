#include "nfc_cli_i.h"

#include <nfc/nfc_scanner.h>

static const char* protocol_names[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = "Iso14443-3a",
    [NfcProtocolIso14443_3b] = "Iso14443-3b",
    [NfcProtocolIso14443_4a] = "Iso14443-4a",
    [NfcProtocolIso14443_4b] = "Iso14443-4b",
    [NfcProtocolIso15693_3] = "Iso15693-3",
    [NfcProtocolFelica] = "FeliCa",
    [NfcProtocolMfUltralight] = "Mifare Ultralight",
    [NfcProtocolMfClassic] = "Mifare Classic",
    [NfcProtocolMfDesfire] = "Mifare DESFire",
    [NfcProtocolSlix] = "Slix",
    [NfcProtocolSt25tb] = "St25tb",
};

#define NFC_CLI_SCANNER_FLAG_DETECTED (1UL << 0)

typedef struct {
    NfcScanner* scanner;
    size_t protocols_detected_num;
    NfcProtocol protocols_detected[NfcProtocolNum];
    FuriThreadId thread_id;
} NfcCliSanner;

static NfcCliSanner* nfc_cli_scanner_alloc(Nfc* nfc) {
    NfcCliSanner* instance = malloc(sizeof(NfcCliSanner));
    instance->thread_id = furi_thread_get_current_id();
    instance->scanner = nfc_scanner_alloc(nfc);

    return instance;
}

static void nfc_cli_scanner_free(NfcCliSanner* instance) {
    nfc_scanner_free(instance->scanner);
    free(instance);
}

static void nfc_cli_detect_scanner_callback(NfcScannerEvent event, void* context) {
    NfcCliSanner* instance = context;

    if(event.type == NfcScannerEventTypeDetected) {
        instance->protocols_detected_num = event.data.protocol_num;
        memcpy(
            instance->protocols_detected,
            event.data.protocols,
            event.data.protocol_num * sizeof(NfcProtocol));
        furi_thread_flags_set(instance->thread_id, NFC_CLI_SCANNER_FLAG_DETECTED);
    }
}

void nfc_cli_scanner(NfcCli* instance, FuriString* args) {
    UNUSED(args);

    NfcCliSanner* cli_scanner = nfc_cli_scanner_alloc(instance->nfc);

    nfc_scanner_start(cli_scanner->scanner, nfc_cli_detect_scanner_callback, cli_scanner);
    printf("Press Ctrl+C to abort\r\n\n");
    while(!nfc_cli_abort_received(instance)) {
        uint32_t event =
            furi_thread_flags_wait(NFC_CLI_SCANNER_FLAG_DETECTED, FuriFlagWaitAny, 50);
        if(event == NFC_CLI_SCANNER_FLAG_DETECTED) break;
    }
    nfc_scanner_stop(cli_scanner->scanner);

    size_t protocols_detected = cli_scanner->protocols_detected_num;
    if(protocols_detected) {
        printf("Detected %zu protocols:\r\n", protocols_detected);
        for(size_t i = 0; i < protocols_detected; i++) {
            printf("%s\r\n", protocol_names[cli_scanner->protocols_detected[i]]);
        }
    }

    nfc_cli_scanner_free(cli_scanner);
}
