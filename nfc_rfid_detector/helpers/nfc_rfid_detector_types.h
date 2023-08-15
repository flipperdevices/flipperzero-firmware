#pragma once

#include <furi.h>
#include <furi_hal.h>

#define NFC_RFID_DETECTOR_VERSION_APP "0.1"
#define NFC_RFID_DETECTOR_DEVELOPED "SkorP"
#define NFC_RFID_DETECTOR_GITHUB "https://github.com/flipperdevices/flipperzero-good-faps"

typedef enum {
    NfcRfidDetectorViewVariableItemList,
    NfcRfidDetectorViewSubmenu,
    NfcRfidDetectorViewFieldPresence,
    NfcRfidDetectorViewWidget,
} NfcRfidDetectorView;
