#include <furi.h>
#include <storage/storage.h>

// Define log tag
#define TAG "example_apps_data"

bool write_file_for_current_app(Storage* storage) {
    bool result = false;

    // Allocate file
    File* file = storage_file_alloc(storage);

    do {
        // Get the path to the current application data folder
        // That is: /ext/apps_data/<app_name>
        // And it will create folders in the path if they don't exist

        // Open file, write data and close it
        if(!storage_file_open(file, "/app/test.txt", FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
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

    // Deallocate file
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
        if(!write_file_for_current_app(storage)) {
            FURI_LOG_E(TAG, "Failed to write file for current app");
            break;
        }

        // Check if file "test.txt" exists
        if(!storage_file_exists(storage, APPSDATA_PATH "/example_apps_data/test.txt")) {
            FURI_LOG_E(TAG, "File 'test.txt' doesn't exist");
            break;
        }

        result = 0;
    } while(0);

    furi_record_close(RECORD_STORAGE);

    return result;
}
