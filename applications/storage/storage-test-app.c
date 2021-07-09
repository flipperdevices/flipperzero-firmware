#include <furi.h>
#include <filesystem-api-v2.h>

#define TAG "storage test"

int32_t storage_app_test(void* p) {
    FS_Api* api = furi_record_open("storage");

    File file;
    bool result =
        api->file.open(api->context, &file, "/ext/test.txt", FSAM_READ, FSOM_OPEN_EXISTING);

    if(result) {
        FURI_LOG_I(TAG, "file opened");
    } else {
        FURI_LOG_E(TAG, "file cannot be open, %d", file.error_id);
    }

    result = api->file.close(api->context, &file);

    if(result) {
        FURI_LOG_I(TAG, "file closed");
    } else {
        FURI_LOG_E(TAG, "file cannot be close %d", file.error_id);
    }

    return 0;
}