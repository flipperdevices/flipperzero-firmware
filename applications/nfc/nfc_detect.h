#pragma once

#include <gui/view.h>

typedef struct NfcDetect NfcDetect;

NfcDetect* nfc_detect_alloc();

void nfc_detect_free(NfcDetect* nfc_detect);

View* nfc_detect_get_view(NfcDetect* nfc_detect);
