#include <furi.h>
#include <api-hal.h>
#include <filesystem-api-v2.h>

#define TAG "storage-test"
#define BYTES_COUNT 16
#define TEST_STRING "123456789012345678901234567890"

static void do_test(FS_Api* api, const char* path) {
    File file;
    bool result;
    uint8_t bytes[BYTES_COUNT + 1];
    uint8_t bytes_count;

    // open
    result = api->file.open(api->context, &file, path, FSAM_WRITE, FSOM_CREATE_ALWAYS);
    if(result) {
        FURI_LOG_I(TAG, "file %s opened", path);
    } else {
        FURI_LOG_E(TAG, "file %s cannot be open, %d", path, file.error_id);
    }

    // write
    bytes_count = api->file.write(api->context, &file, TEST_STRING, strlen(TEST_STRING));
    FURI_LOG_I(TAG, "file writed: %u bytes data = \"%s\"", bytes_count, TEST_STRING);

    // close
    result = api->file.close(api->context, &file);
    if(result) {
        FURI_LOG_I(TAG, "file closed");
    } else {
        FURI_LOG_E(TAG, "file cannot be closed %d", file.error_id);
    }

    // open
    result = api->file.open(api->context, &file, path, FSAM_READ, FSOM_OPEN_EXISTING);
    if(result) {
        FURI_LOG_I(TAG, "file %s opened", path);
    } else {
        FURI_LOG_E(TAG, "file %s cannot be open, %d", path, file.error_id);
    }

    // read
    memset(bytes, 0, BYTES_COUNT + 1);
    bytes_count = api->file.read(api->context, &file, bytes, BYTES_COUNT);
    FURI_LOG_I(TAG, "file readed: %u bytes data = \"%s\"", bytes_count, bytes);

    // close
    result = api->file.close(api->context, &file);
    if(result) {
        FURI_LOG_I(TAG, "file closed");
    } else {
        FURI_LOG_E(TAG, "file cannot be closed %d", file.error_id);
    }
}

int32_t storage_app_test(void* p) {
    FS_Api* api = furi_record_open("storage");

    do_test(api, "/int/test.txt");
    do_test(api, "/any/test.txt");
    do_test(api, "/ext/test.txt");

    while(true) {
        delay(1000);
    }

    return 0;
}