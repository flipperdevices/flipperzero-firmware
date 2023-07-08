#include "filelogger.h"
#include <core/check.h>
#include <core/mutex.h>
#include <furi_hal.h>
#include <storage/storage.h>

#define FURI_LOG_LEVEL_DEFAULT FuriLogLevelInfo

typedef struct {
    FuriLogLevel log_level;
    FuriLogPuts puts;
    FuriLogTimestamp timestamp;
    FuriMutex* mutex;
    File *logFile;
    Storage *storage;
} FuriLogParams;

static FuriLogParams furi_log;

void writeToLog(const char *msg) {
    furi_hal_console_puts(msg);

    // Storage *s = furi_record_open(RECORD_STORAGE);
    // File *f = storage_file_alloc(s);
    // storage_file_open(f, "/any/log.txt", FSAM_READ_WRITE, FSOM_OPEN_APPEND);
    storage_file_write(furi_log.logFile, msg, strlen(msg));
    // storage_file_close(f);
    // storage_file_free(f);
    // furi_record_close(RECORD_STORAGE);
}



void file_log_init() {
    // Set default logging parameters
    furi_log.log_level = FURI_LOG_LEVEL_DEFAULT;
    furi_log.puts = writeToLog;
    furi_log.timestamp = furi_get_tick;
    furi_log.mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    furi_log.storage = furi_record_open(RECORD_STORAGE);
    furi_log.logFile = storage_file_alloc(furi_log.storage);    
    storage_file_open(furi_log.logFile, "/any/log.txt", FSAM_READ_WRITE, FSOM_OPEN_APPEND);
}


void file_log_deinit() {
    storage_file_close(furi_log.logFile);
    storage_file_free(furi_log.logFile);
    furi_record_close(RECORD_STORAGE);
    furi_mutex_free(furi_log.mutex);
}

void file_log_print_format(FuriLogLevel level, const char* tag, const char* format, ...) {
    if(level <= furi_log_get_level() &&
       furi_mutex_acquire(furi_log.mutex, FuriWaitForever) == FuriStatusOk) {
        FuriString* string;
        string = furi_string_alloc();

        const char* log_letter = " ";
        switch(level) {
        case FuriLogLevelError:
            log_letter = "E";
            break;
        case FuriLogLevelWarn:
            log_letter = "W";
            break;
        case FuriLogLevelInfo:
            log_letter = "I";
            break;
        case FuriLogLevelDebug:
            log_letter = "D";
            break;
        case FuriLogLevelTrace:
            log_letter = "T";
            break;
        default:
            break;
        }

        // Timestamp
        furi_string_printf(
            string,
            "%lu [%s][%s] ",
            furi_log.timestamp(),
            log_letter,
            tag);
        furi_log.puts(furi_string_get_cstr(string));
        furi_string_reset(string);

        va_list args;
        va_start(args, format);
        furi_string_vprintf(string, format, args);
        va_end(args);

        furi_log.puts(furi_string_get_cstr(string));
        furi_string_free(string);

        furi_log.puts("\r\n");

        furi_mutex_release(furi_log.mutex);
    }
}


void file_log_set_timestamp(FuriLogTimestamp timestamp) {
    furi_assert(timestamp);
    furi_log.timestamp = timestamp;
}
