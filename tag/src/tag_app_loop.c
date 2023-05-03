#include "tag_app_loop.h"
#include "tag_app.h"

void tag_app_game_loop_run(TagAppState* state, uint32_t duration_s) {
    view_port_update(state->view_port); // make an initial update before the loop starts
    uint32_t start = furi_hal_rtc_get_timestamp();

    FURI_LOG_I(TAG, "Starting run loop...");
    TagEvent event;
    state->running = true;
    while(state->running) {
        switch(furi_message_queue_get(state->queue, &event, (uint32_t)500)) {
        case FuriStatusOk:
            FURI_LOG_I(TAG, "Event from queue: %d", event.type);
            bool updated = false;
            switch(event.type) {
            case TagEventTypeInput:
                // TODO - handle input event
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
            FURI_LOG_E(
                TAG, "furi_message_queue_get was not FuriStatusOk or FuriStatusErrorTimeout");
            state->running = false;
            state->mode = TagAppModeError;
        }

        // TODO: this is for testing - remove when there are end conditions for the app
        // if duration_s > 0 secs then check for the finish time
        if(state->running && duration_s > 0) {
            uint32_t now = furi_hal_rtc_get_timestamp();
            uint32_t duration = now - start;
            if(duration > duration_s) {
                FURI_LOG_I(TAG, "Finishing the run loop");
                state->running = false;
            }
        } // duration check
    } // run loop
    FURI_LOG_I(TAG, "Run loop completed");
}
