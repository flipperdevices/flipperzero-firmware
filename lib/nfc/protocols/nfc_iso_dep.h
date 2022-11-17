#pragma once

#include "nfca.h"
#include <rfal_isoDep.h>

typedef struct {
    NfcaData nfc_data;
    rfalIsoDepDevice iso_dev;
} NfcIsoDep;

bool nfc_iso_dep_check_tag(uint8_t sak);

bool nfc_iso_dep_nfca_activate(NfcIsoDep* nfc_iso_dep);

void nfc_iso_dep_deactivate();
