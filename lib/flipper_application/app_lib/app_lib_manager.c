#include "app_lib_manager.h"

#include <loader/firmware_api/firmware_api.h>
#include <storage/storage.h>
#include <toolbox/path.h>

#include <m-array.h>
#include <m-algo.h>

#include <furi.h>

#define TAG "libmgr"

// ARRAY_DEF(FlipperApplicationLibList, const FlipperApplicationLibraryDescriptor*, M_POD_OPLIST)

ARRAY_DEF(FlipperApplicationList, FlipperApplication*, M_PTR_OPLIST)
#define M_OPL_FlipperApplicationList_t() ARRAY_OPLIST(FlipperApplicationList, M_PTR_OPLIST)

struct ApplicationLibManager {
    const char* application_id;
    uint32_t api_version;
    Storage* storage;
    FlipperApplicationList_t libs;
    const ElfApiInterface* api_interface;
};

ApplicationLibManager* application_lib_manager_alloc(
    const char* application_id,
    uint32_t api_version,
    const ElfApiInterface* api_interface) {
    ApplicationLibManager* manager = malloc(sizeof(ApplicationLibManager));
    manager->application_id = application_id;
    manager->api_version = api_version;
    if(!api_interface) {
        manager->api_interface = firmware_api_interface;
    } else {
        manager->api_interface = api_interface;
    }
    manager->storage = furi_record_open(RECORD_STORAGE);
    FlipperApplicationList_init(manager->libs);
    return manager;
}

void application_lib_manager_free(ApplicationLibManager* manager) {
    // Iterate over all libs and free them
    for
        M_EACH(loaded_lib, manager->libs, FlipperApplicationList_t) {
            // UNUSED(loaded_lib);
            flipper_application_free(*loaded_lib);
        }
    FlipperApplicationList_clear(manager->libs);
    furi_record_close(RECORD_STORAGE);
    free(manager);
}

ApplicationLibManagerError
    application_lib_manager_load_single(ApplicationLibManager* manager, const char* path) {
    FlipperApplication* lib = flipper_application_alloc(manager->storage, manager->api_interface);

    ApplicationLibManagerError error = ApplicationLibManagerErrorNone;
    do {
        FlipperApplicationPreloadStatus preload_res = flipper_application_preload(lib, path);

        if(preload_res != FlipperApplicationPreloadStatusSuccess) {
            FURI_LOG_E(TAG, "Failed to preload %s", path);
            error = ApplicationLibManagerErrorLoaderError;
            break;
        }

        if(!flipper_application_is_lib(lib)) {
            FURI_LOG_E(TAG, "Not a library %s", path);
            error = ApplicationLibManagerErrorLoaderError;
            break;
        }

        FlipperApplicationLoadStatus load_status = flipper_application_map_to_memory(lib);
        if(load_status != FlipperApplicationLoadStatusSuccess) {
            FURI_LOG_E(TAG, "Failed to load module_demo_plugin1.fal");
            break;
        }

        const FlipperApplicationLibraryDescriptor* app_descriptor =
            flipper_application_lib_get(lib);

        if(!app_descriptor) {
            FURI_LOG_E(TAG, "Failed to get descriptor %s", path);
            error = ApplicationLibManagerErrorLoaderError;
            break;
        }

        if(strcmp(app_descriptor->appid, manager->application_id) != 0) {
            FURI_LOG_E(TAG, "Application id mismatch %s", path);
            error = ApplicationLibManagerErrorApplicationIdMismatch;
            break;
        }

        if(app_descriptor->ep_api_version != manager->api_version) {
            FURI_LOG_E(TAG, "API version mismatch %s", path);
            error = ApplicationLibManagerErrorAPIVersionMismatch;
            break;
        }

        FlipperApplicationList_push_back(manager->libs, lib);
    } while(false);

    if(error != ApplicationLibManagerErrorNone) {
        flipper_application_free(lib);
    }

    return error;
}

ApplicationLibManagerError
    application_lib_manager_load_all(ApplicationLibManager* manager, const char* path) {
    // Iterate over all files in the directory
    // UNUSED(manager);
    // UNUSED(path);
    // Iterate over all files in the directory
    File* directory = storage_file_alloc(manager->storage);
    char file_name_buffer[256];
    FuriString* file_name = furi_string_alloc();
    do {
        if(!storage_dir_open(directory, path)) {
            FURI_LOG_E(TAG, "Failed to open directory %s", path);
            break;
        }
        while(true) {
            if(!storage_dir_read(directory, NULL, file_name_buffer, sizeof(file_name_buffer))) {
                break;
            }

            furi_string_set(file_name, file_name_buffer);
            if(!furi_string_end_with_str(file_name, ".fal")) {
                continue;
            }

            path_concat(path, file_name_buffer, file_name);
            FURI_LOG_I(TAG, "Loading %s", furi_string_get_cstr(file_name));
            ApplicationLibManagerError error =
                application_lib_manager_load_single(manager, furi_string_get_cstr(file_name));

            if(error != ApplicationLibManagerErrorNone) {
                FURI_LOG_E(TAG, "Failed to load %s", furi_string_get_cstr(file_name));
                break;
            }
        }
    } while(false);
    storage_dir_close(directory);
    storage_file_free(directory);
    furi_string_free(file_name);
    return ApplicationLibManagerErrorNone;
}

uint32_t application_lib_manager_get_count(ApplicationLibManager* manager) {
    return FlipperApplicationList_size(manager->libs);
}

const FlipperApplicationLibraryDescriptor*
    application_lib_manager_get(ApplicationLibManager* manager, uint32_t index) {
    FlipperApplication* app = *FlipperApplicationList_get(manager->libs, index);
    return flipper_application_lib_get(app);
}

const void* application_lib_manager_get_ep(ApplicationLibManager* manager, uint32_t index) {
    const FlipperApplicationLibraryDescriptor* lib_descr =
        application_lib_manager_get(manager, index);
    furi_check(lib_descr);
    return lib_descr->entry_point;
}
