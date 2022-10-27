#pragma once

#include <lib/toolbox/saved_struct.h>
#include <furi.h>
#include <storage/storage.h>

typedef struct {
    bool mfc_nonce_logging;
} NfcSettings;

bool nfc_settings_load(NfcSettings* nfc_settings);

bool nfc_settings_save(NfcSettings* nfc_settings);