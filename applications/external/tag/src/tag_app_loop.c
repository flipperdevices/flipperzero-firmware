#include "tag_app_loop.h"
#include "tag_app.h"
#include "tag_ir.h"

static void tag_app_start_playing(TagAppState* state) {
    furi_mutex_acquire(state->data_mutex, FuriWaitForever);
    tag_ir_rx_start(tag_ir_callback_decode_to_queue, state->queue);
    state->mode = TagAppModePlaying;
    furi_mutex_release(state->data_mutex);
}

static void tag_app_stop_playing(TagAppState* state) {
    furi_mutex_acquire(state->data_mutex, FuriWaitForever);
    tag_ir_rx_stop();
    state->mode = TagAppModeFinished;
    furi_mutex_release(state->data_mutex);
}

static void tag_app_quit(TagAppState* state) {
    furi_mutex_acquire(state->data_mutex, FuriWaitForever);
    if(state->mode == TagAppModePlaying) tag_ir_rx_stop();
    state->mode = TagAppModeQuit;
    furi_mutex_release(state->data_mutex);
}

static void tag_app_error(TagAppState* state) {
    furi_mutex_acquire(state->data_mutex, FuriWaitForever);
    if(state->mode == TagAppModePlaying) tag_ir_rx_stop();
    state->mode = TagAppModeError;
    furi_mutex_release(state->data_mutex);
}

static bool tag_app_handle_input(TagAppState* state, InputEvent input) {
    if(input.type == InputTypeShort) {
        if(input.key == InputKeyOk) {
            if(state->mode == TagAppModeReady) {
                tag_app_start_playing(state);
                return true;
            }
            if(state->mode == TagAppModeFinished) {
                tag_app_start_playing(state);
                return true;
            }
            // TODO: when playing OK means something else
        }
        if(input.key == InputKeyBack) {
            if(state->mode == TagAppModePlaying) {
                tag_app_stop_playing(state);
                return true;
            }
        }
    }
    if(input.type == InputTypeLong && input.key == InputKeyBack) {
        tag_app_quit(state);
        return false; // don't try to update the ui while quitting
    }

    return false;
}

void tag_app_game_loop_run(TagAppState* state, uint32_t duration_s) {
    view_port_update(state->view_port); // make an initial update before the loop starts
    uint32_t start = furi_hal_rtc_get_timestamp();

    FURI_LOG_I(TAG, "Starting run loop...");
    TagEvent event;
    state->running = true;
    while(state->running) {
        switch(furi_message_queue_get(state->queue, &event, 500)) {
        case FuriStatusOk:
            FURI_LOG_I(TAG, "Event from queue: %d", event.type);
            bool updated = false;
            switch(event.type) {
            case TagEventTypeInput:
                updated = tag_app_handle_input(state, event.input);
                break;
            case TagEventTypeInfraredMessage:
                // TODO - handle ir message
                break;
            case TagEventTypeSubGhzDataDetected:
                // TODO - fetch subghz data, and push as a new message into the queue
                break;
            default:
                FURI_LOG_E(TAG, "Unknown event received from queue.");
                break;
            }
            if(updated) {
                view_port_update(state->view_port);
            }
            break;

        // error cases
        case FuriStatusErrorTimeout:
            FURI_LOG_D(TAG, "furi_message_queue_get timed out");
            break;
        default:
            FURI_LOG_E(TAG, "furi_message_queue_get was neither ok nor timeout");
            tag_app_error(state);
        }

        // TODO: this is for testing - remove when there are end conditions for the app
        // if duration_s > 0 secs then check for the finish time
        if(state->running && duration_s > 0) {
            uint32_t now = furi_hal_rtc_get_timestamp();
            uint32_t duration = now - start;
            if(duration > duration_s) {
                FURI_LOG_I(TAG, "Finishing the run loop");
                tag_app_quit(state);
            }
        } // duration check

        if(state->mode == TagAppModeQuit || state->mode == TagAppModeError) {
            state->running = false;
        }
    } // run loop
    FURI_LOG_I(TAG, "Run loop completed");
}
