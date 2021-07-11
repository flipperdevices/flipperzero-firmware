#include <furi.h>
#include <api-hal.h>
#include <filesystem-api-v2.h>

#define TAG "storage-test"
#define BYTES_COUNT 16
#define TEST_STRING "TestDataStringProvidedByDiceRoll"
#define SEEK_OFFSET_FROM_START 10
#define SEEK_OFFSET_INCREASE 12
#define SEEK_OFFSET_SUM (SEEK_OFFSET_FROM_START + SEEK_OFFSET_INCREASE)

static void do_file_test(FS_Api* api, const char* path) {
    File file;
    bool result;
    uint8_t bytes[BYTES_COUNT + 1];
    uint8_t bytes_count;
    uint64_t position;
    uint64_t size;

    FURI_LOG_I(TAG, "--------- \"%s\" ---------", path);

    // open
    result = api->file.open(api->context, &file, path, FSAM_WRITE, FSOM_CREATE_ALWAYS);
    if(result) {
        FURI_LOG_I(TAG, "file open");
    } else {
        FURI_LOG_E(TAG, "file open, %s", api->error.get_desc(api->context, file.error_id));
    }

    // write
    bytes_count = api->file.write(api->context, &file, TEST_STRING, strlen(TEST_STRING));
    if(bytes_count == 0) {
        FURI_LOG_E(TAG, "file write, %s", api->error.get_desc(api->context, file.error_id));
    } else {
        FURI_LOG_I(TAG, "file write");
    }

    // sync
    result = api->file.sync(api->context, &file);
    if(result) {
        FURI_LOG_I(TAG, "file sync");
    } else {
        FURI_LOG_E(TAG, "file sync, %s", api->error.get_desc(api->context, file.error_id));
    }

    // eof #1
    result = api->file.eof(api->context, &file);
    if(result) {
        FURI_LOG_I(TAG, "file eof #1");
    } else {
        FURI_LOG_E(TAG, "file eof #1, %s", api->error.get_desc(api->context, file.error_id));
    }

    // seek from start and tell
    result = api->file.seek(api->context, &file, SEEK_OFFSET_FROM_START, true);
    if(result) {
        FURI_LOG_I(TAG, "file seek #1");
    } else {
        FURI_LOG_E(TAG, "file seek #1, %s", api->error.get_desc(api->context, file.error_id));
    }
    position = api->file.tell(api->context, &file);
    if(position != SEEK_OFFSET_FROM_START) {
        FURI_LOG_E(TAG, "file tell #1, %s", api->error.get_desc(api->context, file.error_id));
    } else {
        FURI_LOG_I(TAG, "file tell #1");
    }

    // size
    size = api->file.size(api->context, &file);
    if(size != strlen(TEST_STRING)) {
        FURI_LOG_E(TAG, "file size #1, %s", api->error.get_desc(api->context, file.error_id));
    } else {
        FURI_LOG_I(TAG, "file size #1");
    }

    // seek and tell
    result = api->file.seek(api->context, &file, SEEK_OFFSET_INCREASE, false);
    if(result) {
        FURI_LOG_I(TAG, "file seek #2");
    } else {
        FURI_LOG_E(TAG, "file seek #2, %s", api->error.get_desc(api->context, file.error_id));
    }
    position = api->file.tell(api->context, &file);
    if(position != SEEK_OFFSET_SUM) {
        FURI_LOG_E(TAG, "file tell #2, %s", api->error.get_desc(api->context, file.error_id));
    } else {
        FURI_LOG_I(TAG, "file tell #2");
    }

    // eof #2
    result = api->file.eof(api->context, &file);
    if(!result) {
        FURI_LOG_I(TAG, "file eof #2");
    } else {
        FURI_LOG_E(TAG, "file eof #2, %s", api->error.get_desc(api->context, file.error_id));
    }

    // truncate
    result = api->file.truncate(api->context, &file);
    if(result) {
        FURI_LOG_I(TAG, "file truncate");
    } else {
        FURI_LOG_E(TAG, "file truncate, %s", api->error.get_desc(api->context, file.error_id));
    }
    size = api->file.size(api->context, &file);
    if(size != SEEK_OFFSET_SUM) {
        FURI_LOG_E(TAG, "file size #2, %s", api->error.get_desc(api->context, file.error_id));
    } else {
        FURI_LOG_I(TAG, "file size #2");
    }

    // close
    result = api->file.close(api->context, &file);
    if(result) {
        FURI_LOG_I(TAG, "file close");
    } else {
        FURI_LOG_E(TAG, "file close, %s", api->error.get_desc(api->context, file.error_id));
    }

    // open
    result = api->file.open(api->context, &file, path, FSAM_READ, FSOM_OPEN_EXISTING);
    if(result) {
        FURI_LOG_I(TAG, "file open");
    } else {
        FURI_LOG_E(TAG, "file open, %s", api->error.get_desc(api->context, file.error_id));
    }

    // read
    memset(bytes, 0, BYTES_COUNT + 1);
    bytes_count = api->file.read(api->context, &file, bytes, BYTES_COUNT);
    if(bytes_count == 0) {
        FURI_LOG_E(TAG, "file read, %s", api->error.get_desc(api->context, file.error_id));
    } else {
        if(memcmp(TEST_STRING, bytes, bytes_count) == 0) {
            FURI_LOG_I(TAG, "file read");
        } else {
            FURI_LOG_E(TAG, "file read, garbage");
        }
    }

    // close
    result = api->file.close(api->context, &file);
    if(result) {
        FURI_LOG_I(TAG, "file close");
    } else {
        FURI_LOG_E(TAG, "file close, %s", api->error.get_desc(api->context, file.error_id));
    }
}

int32_t storage_app_test(void* p) {
    FS_Api* api = furi_record_open("storage");

    do_file_test(api, "/int/test.txt");
    do_file_test(api, "/any/test.txt");
    do_file_test(api, "/ext/test.txt");

    while(true) {
        delay(1000);
    }

    return 0;
}