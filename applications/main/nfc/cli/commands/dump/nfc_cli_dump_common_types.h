#pragma once

#include <furi.h>

//#include "../../../helpers/nfc_detected_protocols.h"
#include "../../../helpers/mf_classic_key_cache.h"
#include "../helpers/nfc_cli_scanner.h"

#include <nfc/nfc.h>
#include <nfc/protocols/nfc_protocol.h>
#include <nfc/nfc_device.h>
#include <nfc/nfc_poller.h>

#include <nfc/protocols/felica/felica.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight.h>

#include <storage/storage.h>

#define NFC_CLI_DUMP_KEY_MAX_SIZE (16)

typedef union {
    MfUltralightAuthPassword password;
    FelicaCardKey felica_key;
    MfUltralightC3DesAuthKey tdes_key;
    uint8_t key[NFC_CLI_DUMP_KEY_MAX_SIZE];
} NfcCliDumpKeyUnion;

typedef enum {
    NfcCliDumpErrorNone,
    NfcCliDumpErrorTimeout,
    NfcCliDumpErrorNotPresent,
    NfcCliDumpErrorFailedToRead,
} NfcCliDumpError;

typedef struct {
    Nfc* nfc;
    FuriString* file_path;
    Storage* storage;
    NfcCliScanner* scanner;
    //NfcDetectedProtocols* detected_protocols;

    FuriSemaphore* sem_done;

    NfcCliDumpError result;
    FuriString* result_string;

    NfcCliDumpKeyUnion key;
    MfClassicKeyCache* mfc_key_cache;

    NfcPoller* poller;
    NfcDevice* nfc_device;
} NfcCliDumpContext;
