#pragma once

// #include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct ValidatorIsFile ValidatorIsFile;

ValidatorIsFile* validator_is_file_alloc(
    const char* app_path_folder,
    const char* app_file_name,
    const char* app_extension);

void validator_is_file_free(ValidatorIsFile* instance);

bool validator_is_file_callback(void* context);

#ifdef __cplusplus
}
#endif