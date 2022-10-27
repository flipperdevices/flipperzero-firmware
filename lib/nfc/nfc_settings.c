#include <nfc_settings.h>

#define NFC_SETTINGS_FILE_NAME ".nfc.settings"
#define NFC_SETTINGS_PATH INT_PATH(NFC_SETTINGS_FILE_NAME)
#define NFC_SETTINGS_VERSION (0)
#define NFC_SETTINGS_MAGIC (0x23)

bool nfc_settings_load(NfcSettings* nfc_settings) {
    furi_assert(nfc_settings);

    return saved_struct_load(
        NFC_SETTINGS_PATH,
        nfc_settings,
        sizeof(NfcSettings),
        NFC_SETTINGS_MAGIC,
        NFC_SETTINGS_VERSION);
}

bool nfc_settings_save(NfcSettings* nfc_settings) {
    furi_assert(nfc_settings);

    return saved_struct_save(
        NFC_SETTINGS_PATH,
        nfc_settings,
        sizeof(NfcSettings),
        NFC_SETTINGS_MAGIC,
        NFC_SETTINGS_VERSION);
}