#include "nfc_app_api.h"

/* Actual implementation of app's API and its private state */

void nfc_app_api_gallagher_deobfuscate_and_parse_credential(
    GallagherCredential* credential,
    const uint8_t* cardholder_data_obfuscated) {
    // Straight callthrough to private implementation
    return gallagher_deobfuscate_and_parse_credential(credential, cardholder_data_obfuscated);
}

const uint8_t (*NFC_APP_API_GALLAGHER_CARDAX_ASCII)[MF_CLASSIC_BLOCK_SIZE] =
    &GALLAGHER_CARDAX_ASCII;