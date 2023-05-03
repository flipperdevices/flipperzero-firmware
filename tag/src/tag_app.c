#include "tag_app.h"

static TagAppState* state;

TagAppState* tag_app_state_get() {
    return state;
}

void tag_app_init() {
    state = malloc(sizeof(state));
    state->app_mode = TagAppModeUninitialised;
    state->queue = furi_message_queue_alloc(8, sizeof(TagEvent));
}

void tag_app_destroy() {
    furi_message_queue_free(state->queue);
    free(state);
}

void tag_app_start_playing() {
    // TODO
}

void tag_app_stop_playing() {
    // TODO - stop playing
}

void test_tx() {
    FURI_LOG_I(TAG, "Creating message for address: 0x4, command: 0x11");
    InfraredMessage* message = tag_ir_create_firing_message(0x11);
    FURI_LOG_I(TAG, "Sending message (5 repeats)");
    tag_ir_send(message);
    FURI_LOG_I(TAG, "Freeing message");
    tag_ir_free_message(message);
}

void queue_spin(uint32_t duration_s) {
    uint32_t start = furi_hal_rtc_get_timestamp();
    TagEvent received;
    state->running = true;
    while(state->running) {
        switch(furi_message_queue_get(state->queue, &received, (uint32_t)500)) {
        case FuriStatusOk:
            FURI_LOG_I(TAG, "Event from queue: %d", received.type);
            // TODO: do something with this event
            break;
        case FuriStatusErrorTimeout:
            FURI_LOG_I(TAG, "Queue timed out.");
            break;
        default:
            FURI_LOG_E(
                TAG, "furi_message_queue_get was not FuriStatusOk or FuriStatusErrorTimeout");
            state->running = false;
        }

        // check for the end of the queue
        uint32_t now = furi_hal_rtc_get_timestamp();
        uint32_t duration = now - start;
        if(duration > duration_s) {
            FURI_LOG_I(TAG, "Finishing with the queue");
            state->running = false;
        }
    }
}

int32_t tag_game_app(void* p) {
    UNUSED(p);

    FURI_LOG_I(TAG, "Initialising app");
    tag_app_init();
    tag_ir_init(InfraredProtocolNEC, 5, 0x4);

    test_tx();

    tag_ir_rx_start(tag_ir_callback_decode_to_queue, state->queue);
    queue_spin(10);
    tag_ir_rx_stop();

    FURI_LOG_I(TAG, "Tearing down app");
    tag_ir_destroy();
    tag_app_destroy();
    return 0;
}
