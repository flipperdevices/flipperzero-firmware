#include <furi.h>
#include "path.h"
#include "path_helper.h"
#include "storage/storage.h"

struct PathHelper {
    FuriString* path;
};

PathHelper* path_helper_alloc() {
    PathHelper* path_helper = malloc(sizeof(PathHelper));
    path_helper->path = furi_string_alloc();
    return path_helper;
}

PathHelper* path_helper_alloc_apps_data() {
    PathHelper* path_helper = path_helper_alloc();
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_common_get_my_data_path(storage, path_helper->path);
    furi_record_close(RECORD_STORAGE);
    return path_helper;
}

void path_helper_free(PathHelper* path_helper) {
    furi_string_free(path_helper->path);
    free(path_helper);
}

void path_helper_append(PathHelper* path_helper, const char* path) {
    path_append(path_helper->path, path);
}

const char* path_helper_get(PathHelper* path_helper) {
    return furi_string_get_cstr(path_helper->path);
}