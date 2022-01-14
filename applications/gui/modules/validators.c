//#include <gui/elements.h>
#include <furi.h>
#include "validators.h"
#include "applications/storage/storage.h"

struct ValidatorIsFile {
    const char* app_path_folder;
    const char* app_file_name;
    const char* app_extension;
};

bool validator_is_file_callback(void* context) {
    furi_assert(context);
    ValidatorIsFile* instance = context;
    bool ret = true;
    string_t path;
    string_init_printf(
        path,
        "%s/%s%s",
        instance->app_path_folder,
        instance->app_file_name,
        instance->app_extension);
    Storage* storage = furi_record_open("storage");
    if(storage_common_stat(storage, string_get_cstr(path), NULL) == FSE_OK) {
        ret = false;
    } else {
        ret = true;
    }
    string_clear(path);
    furi_record_close("storage");
    return ret;
}

ValidatorIsFile* validator_is_file_alloc(
    const char* app_path_folder,
    const char* app_file_name,
    const char* app_extension) {
    ValidatorIsFile* instance = furi_alloc(sizeof(ValidatorIsFile));

    instance->app_path_folder = app_path_folder;
    instance->app_file_name = app_file_name;
    instance->app_extension = app_extension;

    return instance;
}

void validator_is_file_free(ValidatorIsFile* instance) {
    furi_assert(instance);
    free(instance);
}

