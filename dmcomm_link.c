#include "dmcomm_link.h"

/*
 * dmcomm thread, runs the dmcomm loop. call init first.
 * exits when the app dmcomm_run var is set to false.
 * App runs this constantly on startup until app close.
*/
int32_t dmcomm_reader(void* context) {
    FURI_LOG_I(TAG, "dmcomm_reader start");
    App* app = context;
    setup();
    while(app->dmcomm_run) {
        loop();
    }

    FURI_LOG_I(TAG, "dmcomm_reader end");
    return 0;
}

/*
 * Send a serial command to the dmcomm thread
*/
void dmcomm_sendcommand(void* context, const char* cmd)
{
    FURI_LOG_I(TAG, "dmcomm_reader start");
    App* app = context;
    
    furi_mutex_acquire(app->dmcomm_mutex, FuriWaitForever);

    size_t sent = furi_stream_buffer_send(
        app->dmcomm_stream_buffer,
        cmd,
        strlen(cmd),
        1);

    if(sent != strlen(cmd))
        FURI_LOG_I(TAG, "partial send %d/%d", sent, strlen(cmd));

    furi_mutex_release(app->dmcomm_mutex);
}


/*
furi_thread_flags_set(furi_thread_get_id(context->reader_thread), ReaderThreadFlagExit);

static int32_t example_thermo_reader_thread_callback(void* ctx) {
    ExampleThermoContext* context = ctx;

    for(;;) {
        example_thermo_request_temperature(context);

        const uint32_t flags =
            furi_thread_flags_wait(ReaderThreadFlagExit, FuriFlagWaitAny, UPDATE_PERIOD_MS);

        if(flags != (unsigned)FuriFlagErrorTimeout) break;

        example_thermo_read_temperature(context);
    }

    return 0;
}*/

/*

struct LFRFIDRawFile {
    Stream* stream;
    uint32_t max_buffer_size;

    uint8_t* buffer;
    uint32_t buffer_size;
    size_t buffer_counter;
};

Storage* storage = furi_record_open(RECORD_STORAGE);

file->stream = file_stream_alloc(storage);
file->buffer = NULL;

READ  file_stream_open(file->stream, file_path, FSAM_READ, FSOM_OPEN_EXISTING);

read data:
size_t size_read = stream_read(file->stream, (uint8_t*)&buffer, sizeof(buffer));


WRITE file_stream_open(file->stream, file_path, FSAM_READ_WRITE, FSOM_CREATE_ALWAYS);
size_t size_wrote = stream_write(file->stream, (uint8_t*)&buffer, sizeof(buffer));

stream_free(file->stream);

furi_record_close(RECORD_STORAGE);
*/