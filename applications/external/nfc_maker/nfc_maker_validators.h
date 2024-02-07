#pragma once

#include <core/common_defines.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct NFCMakerValidatorIsFile NFCMakerValidatorIsFile;

NFCMakerValidatorIsFile* nfc_maker_validator_is_file_alloc_init(
    const char* app_path_folder,
    const char* app_extension,
    const char* current_name);

void nfc_maker_validator_is_file_free(NFCMakerValidatorIsFile* instance);

bool nfc_maker_validator_is_file_callback(const char* text, FuriString* error, void* context);

#ifdef __cplusplus
}
#endif
