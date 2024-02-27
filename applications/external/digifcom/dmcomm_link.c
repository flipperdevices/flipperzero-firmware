#include "dmcomm_link.h"
#include "dmcomm_lib/fcom.h"

/*
 * dmcomm thread, runs the dmcomm loop. call init first.
 * exits when the app dmcomm_run var is set to false.
 * App runs this constantly on startup until app close.
*/
int32_t fcom_thread(void* context) {
    FURI_LOG_I(TAG, "fcom_thread start");
    App* app = context;
    setup_fcom(context);
    while(app->dmcomm_run) {
        loop_fcom();
    }
    destroy_fcom();
    FURI_LOG_I(TAG, "fcom_thread end");
    return 0;
}

/*
Send a serial command to the dmcomm thread

Used for comms by app (non USB)
*/
void dmcomm_sendcommand(void* context, const char* cmd) {
    FURI_LOG_I(TAG, "dmcomm_sendcommand: %s", cmd);
    App* app = context;

    size_t sent = furi_stream_buffer_send(app->dmcomm_input_stream, cmd, strlen(cmd), 0);

    if(sent != strlen(cmd)) // Shouldn't happen
        FURI_LOG_I(TAG, "partial send %d/%d", sent, strlen(cmd));
}

/*
Send binary data, used for USB serial link
*/
void dmcomm_senddata(void* context, uint8_t* data, size_t len) {
    App* app = context;

    size_t sent = furi_stream_buffer_send(app->dmcomm_input_stream, data, len, 0);

    if(sent != len) // Shouldn't happen
        FURI_LOG_I(TAG, "partial send %d/%d", sent, len);
}
