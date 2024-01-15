#pragma once

/* 
 * This file contains an API that is internally implemented by the application
 * It is also exposed to plugins to allow them to use the application's API.
 */
#include <stdint.h>
#include "helpers/gallagher_util.h"

#ifdef __cplusplus
extern "C" {
#endif

void nfc_app_api_gallagher_deobfuscate_and_parse_credential(
    GallagherCredential* credential,
    const uint8_t* cardholder_data_obfuscated);

#ifdef __cplusplus
}
#endif
