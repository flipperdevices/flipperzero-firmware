#include "tag_app_loop.h"
#include "tag_app.h"
#include "tag_ir.h"

static void tag_app_start_playing(TagAppState* state) {
    furi_mutex_acquire(state->data_mutex, FuriWaitForever);
    tag_ir_rx_start(tag_ir_callback_decode_to_queue, state->queue);
    state->data->energy = 0.0; // start empty
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
    if(input.type == InputTypeShort && input.key == InputKeyOk) {
        if(state->mode == TagAppModeReady) {
            tag_app_start_playing(state);
            return true;
        }
        if(state->mode == TagAppModeFinished) {
            tag_app_start_playing(state);
            return true;
        }
        // TODO: when playing, OK means fire
    }

    if(input.type == InputTypeShort && input.key == InputKeyBack) {
        if(state->mode == TagAppModePlaying) {
            tag_app_stop_playing(state);
            return true;
        }
    }

    if(input.type == InputTypeLong && input.key == InputKeyBack) {
        tag_app_quit(state);
        return false; // don't try to update the ui while quitting
    }

    return false;
}

void tag_app_game_loop_recover_energy(TagAppState* state, uint32_t delta) {
    if(state->data->energy < ENERGY_MAX) {
        state->data->energy += ENERGY_MAX * (delta / ENERGY_RECOVERY_ms);
        if(state->data->energy > ENERGY_MAX) state->data->energy = ENERGY_MAX;
    }
}

void tag_app_game_loop_run(TagAppState* state) {
    FURI_LOG_I(TAG, "Starting run loop...");
    view_port_update(state->view_port); // make an initial update before the loop starts
    uint32_t previous_loop_timestamp = furi_hal_rtc_get_timestamp();
    TagEvent event;
    state->running = true;
    while(state->running) {
        uint32_t delta = furi_hal_rtc_get_timestamp() - previous_loop_timestamp;

        // fetch the next message (with a small timeout if none)
        switch(furi_message_queue_get(state->queue, &event, LOOP_MESSAGE_TIMEOUT_ms)) {
        case FuriStatusOk:
            FURI_LOG_I(TAG, "Event from queue: %d", event.type);
            bool updated = false;
            switch(event.type) {
            case TagEventTypeInput:
                updated = tag_app_handle_input(state, event.input);
                break;
            case TagEventTypeInfraredMessage:
                FURI_LOG_D(
                    TAG,
                    "Received ir with address: %lu, command: %lu",
                    event.ir_message->address,
                    event.ir_message->command);
                state->data->last_ir_address = event.ir_message->address;
                state->data->last_ir_command = event.ir_message->command;
                updated = true;
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

        // do things that always happen in each loop
        tag_app_game_loop_recover_energy(state, delta);

        // exit the application if necessary
        if(state->mode == TagAppModeQuit || state->mode == TagAppModeError) {
            state->running = false;
        }

        // prepare for next iteration of the loop
        previous_loop_timestamp += delta;
    } // run loop
    FURI_LOG_I(TAG, "Run loop completed");
}
