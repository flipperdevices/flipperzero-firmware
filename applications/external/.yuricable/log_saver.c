#include <furi.h>
#include <furi_hal.h>
#include <locale/locale.h>
#include <storage/storage.h>

#define TAG "YuriStorage"
#define STORAGE_FILE_BUF_LEN 5
#define END_MARKER "======== End of iBoot serial output. ========"
#define MAX_BUFFER_SIZE 102400

static char aggregate_buffer[MAX_BUFFER_SIZE];
static size_t aggregate_buffer_len = 0;

static bool storage_printf(File* file, const char* format, ...) {
    va_list args;
    va_start(args, format);
    FuriString* fstring = furi_string_alloc_vprintf(format, args);
    va_end(args);
    if(storage_file_write(file, furi_string_get_cstr(fstring), furi_string_size(fstring)) &&
       storage_file_write(file, "\n", 1)) {
        furi_string_free(fstring);
        return true;
    }
    furi_string_free(fstring);
    return false;
}

static bool storage_write_file(File* file, const char* str, size_t str_len) {
    storage_file_seek(file, 0, true);
    storage_file_truncate(file);
    bool result = true;
    if(!storage_printf(file, "%.*s", (int)str_len, str)) {
        result = false;
    }
    return result;
}

void save_log_and_write(char* str, size_t len) {
    if(aggregate_buffer_len + len > MAX_BUFFER_SIZE) {
        aggregate_buffer_len = 0;
    }
    memcpy(aggregate_buffer + aggregate_buffer_len, str, len);
    aggregate_buffer_len += len;
    if(strstr(aggregate_buffer, END_MARKER) != NULL) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        File* file = storage_file_alloc(storage);
        FuriHalRtcDateTime currentDate;
        furi_hal_rtc_get_datetime(&currentDate);
        char dateTimeStr[64];
        snprintf(
            dateTimeStr,
            sizeof(dateTimeStr),
            "iBoot_log_%04u%02u%02u%02u%02u.txt",
            currentDate.year,
            currentDate.month,
            currentDate.day,
            currentDate.hour,
            currentDate.minute);
        char fullPath[128];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", STORAGE_APP_DATA_PATH_PREFIX, dateTimeStr);

        if(!storage_file_open(file, fullPath, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
            FURI_LOG_E(TAG, "Failed to open file");
            storage_file_free(file);
            furi_record_close(RECORD_STORAGE);
            return;
        }

        if(!storage_write_file(file, aggregate_buffer, aggregate_buffer_len)) {
            FURI_LOG_E(TAG, "Failed to write log to file");
        }

        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);
        aggregate_buffer_len = 0;
    }
}