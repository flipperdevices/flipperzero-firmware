#include <furi.h>
#include <storage/storage.h>
#include <toolbox/path_helper.h>

// Define log tag
#define TAG "example_apps_data"

bool write_file_for_current_apps_data_raw(Storage* storage, const char* file_name) {
    bool result = false;

    // Allocate file and path string
    File* file = storage_file_alloc(storage);
    FuriString* path = furi_string_alloc();

    do {
        // Get the path to the current application data folder
        // That is: /ext/apps_data/<app_name>
        // And it will create folders in the path if they don't exist
        if(!storage_common_get_my_data_path(storage, path)) {
            FURI_LOG_E(TAG, "Failed to get current app data path or create folders");
            break;
        }

        // Append file name to path
        furi_string_cat(path, "/");
        furi_string_cat(path, file_name);

        // Open file, write data and close it
        if(!storage_file_open(file, furi_string_get_cstr(path), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
            FURI_LOG_E(TAG, "Failed to open file");
            break;
        }
        if(!storage_file_write(file, "Hello World!", strlen("Hello World!"))) {
            FURI_LOG_E(TAG, "Failed to write to file");
            break;
        }
        storage_file_close(file);

        result = true;
    } while(0);

    // Deallocate path string and file
    furi_string_free(path);
    storage_file_free(file);

    return result;
}

bool write_file_for_current_apps_data_helper(Storage* storage, const char* file_name) {
    bool result = false;

    // Allocate file
    File* file = storage_file_alloc(storage);

    // Allocate PathHelper and set path to current app data folder
    PathHelper* path = path_helper_alloc_apps_data();

    do {
        // Append file name to path
        path_helper_append(path, file_name);

        // Open file, write data and close it
        if(!storage_file_open(file, path_helper_get(path), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
            FURI_LOG_E(TAG, "Failed to open file");
            break;
        }
        if(!storage_file_write(file, "Hello World!", strlen("Hello World!"))) {
            FURI_LOG_E(TAG, "Failed to write to file");
            break;
        }
        storage_file_close(file);

        result = true;
    } while(0);

    // Deallocate PathHelper and file
    path_helper_free(path);
    storage_file_free(file);

    return result;
}

// Application entry point
int32_t example_apps_data_main(void* p) {
    UNUSED(p);
    int32_t result = 1;

    Storage* storage = furi_record_open(RECORD_STORAGE);

    do {
        // Write file "test.txt" to current app data folder
        if(!write_file_for_current_apps_data_raw(storage, "test.txt")) {
            FURI_LOG_E(TAG, "Failed to write file for current app data (raw)");
            break;
        }

        // Write file "test2.txt" to current app data folder using PathHelper
        if(!write_file_for_current_apps_data_helper(storage, "test2.txt")) {
            FURI_LOG_E(TAG, "Failed to write file for current app data (helper)");
            break;
        }

        // Check if file "test.txt" exists
        if(!storage_file_exists(storage, APPSDATA_PATH "/example_apps_data/test.txt")) {
            FURI_LOG_E(TAG, "File 'test.txt' doesn't exist");
            break;
        }

        // Check if file "test2.txt" exists
        if(!storage_file_exists(storage, APPSDATA_PATH "/example_apps_data/test2.txt")) {
            FURI_LOG_E(TAG, "File 'test2.txt' doesn't exist");
            break;
        }

        result = 0;
    } while(0);

    furi_record_close(RECORD_STORAGE);

    return result;
}
