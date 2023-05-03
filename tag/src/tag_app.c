#include "tag_app.h"
#include "tag_ir.h"
#include "tag_subghz.h"
#include "tag_app_loop.h"
#include "tag_ui.h"

static TagAppState* state;

TagAppState* tag_app_state_get() {
    return state;
}

void tag_app_init() {
    state = malloc(sizeof(state));
    state->mode = TagAppModeUninitialised;
    state->queue = furi_message_queue_alloc(8, sizeof(TagEvent));
}

void tag_app_destroy() {
    furi_message_queue_free(state->queue);
    free(state);
}

void test_tx() {
    FURI_LOG_I(TAG, "Creating message for address: 0x4, command: 0x11");
    InfraredMessage* message = tag_ir_create_firing_message(0x11);
    FURI_LOG_I(TAG, "Sending message (5 repeats)");
    tag_ir_send(message);
    FURI_LOG_I(TAG, "Freeing message");
    tag_ir_free_message(message);
}

int32_t tag_game_app(void* p) {
    UNUSED(p);

    FURI_LOG_I(TAG, "Initialising app");
    tag_app_init();
    tag_ui_init(state);
    tag_ir_init(InfraredProtocolNEC, 5, 0x4);
    state->mode = TagAppModeReady;

    test_tx();

    tag_ir_rx_start(tag_ir_callback_decode_to_queue, state->queue);

    tag_app_game_loop_run(state, 30);
    tag_ir_rx_stop();

    FURI_LOG_I(TAG, "Tearing down app");
    tag_ir_destroy();
    tag_ui_destroy(state);
    tag_app_destroy();
    return 0;
}
