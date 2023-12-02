#include "loclass_writer.h"

#include <furi/furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <stream/stream.h>
#include <stream/buffered_file_stream.h>

struct LoclassWriter {
    Stream* file_stream;
};

#define LOCLASS_LOGS_PATH EXT_PATH("apps_data/picopass/.loclass.log")

LoclassWriter* loclass_writer_alloc() {
    LoclassWriter* instance = malloc(sizeof(LoclassWriter));
    Storage* storage = furi_record_open(RECORD_STORAGE);
    instance->file_stream = buffered_file_stream_alloc(storage);
    storage_simply_mkdir(storage, STORAGE_APP_DATA_PATH_PREFIX);
    if(!buffered_file_stream_open(
           instance->file_stream, LOCLASS_LOGS_PATH, FSAM_WRITE, FSOM_OPEN_APPEND)) {
        buffered_file_stream_close(instance->file_stream);
        stream_free(instance->file_stream);
        free(instance);
        instance = NULL;
    }

    furi_record_close(RECORD_STORAGE);

    return instance;
}

void loclass_writer_free(LoclassWriter* instance) {
    furi_assert(instance != NULL);

    buffered_file_stream_close(instance->file_stream);
    stream_free(instance->file_stream);
    free(instance);
}

bool loclass_writer_write_start_stop(LoclassWriter* instance, bool start) {
    furi_assert(instance != NULL);

    FuriHalRtcDateTime curr_dt;
    furi_hal_rtc_get_datetime(&curr_dt);
    uint32_t curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);

    FuriString* str = furi_string_alloc_printf(
        "loclass-v1-info ts %lu %s\n", curr_ts, start ? "started" : "finished");
    bool write_success = stream_write_string(instance->file_stream, str);
    furi_string_free(str);
    return write_success;
}

bool loclass_writer_write_params(
    LoclassWriter* instance,
    uint8_t log_no,
    const uint8_t csn[8],
    const uint8_t epurse[8],
    const uint8_t nr[4],
    const uint8_t mac[4]) {
    furi_assert(instance != NULL);

    FuriHalRtcDateTime curr_dt;
    furi_hal_rtc_get_datetime(&curr_dt);
    uint32_t curr_ts = furi_hal_rtc_datetime_to_timestamp(&curr_dt);

    FuriString* str = furi_string_alloc_printf(
        "loclass-v1-mac ts %lu no %u "
        "csn %02x%02x%02x%02x%02x%02x%02x%02x "
        "cc %02x%02x%02x%02x%02x%02x%02x%02x "
        "nr %02x%02x%02x%02x "
        "mac %02x%02x%02x%02x\n",
        curr_ts,
        log_no,
        csn[0],
        csn[1],
        csn[2],
        csn[3],
        csn[4],
        csn[5],
        csn[6],
        csn[7],
        epurse[0],
        epurse[1],
        epurse[2],
        epurse[3],
        epurse[4],
        epurse[5],
        epurse[6],
        epurse[7],
        nr[0],
        nr[1],
        nr[2],
        nr[3],
        mac[0],
        mac[1],
        mac[2],
        mac[3]);
    bool write_success = stream_write_string(instance->file_stream, str);
    furi_string_free(str);
    return write_success;
}
