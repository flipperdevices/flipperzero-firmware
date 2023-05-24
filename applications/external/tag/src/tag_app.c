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
    FURI_LOG_T(TAG, "tag_app_init");
    state = malloc(sizeof(TagAppState));
    state->mode = TagAppModeUninitialised;
    state->queue = furi_message_queue_alloc(8, sizeof(TagEvent));
    state->data_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    state->data = malloc(sizeof(GameData));
}

void tag_app_destroy() {
    FURI_LOG_T(TAG, "tag_app_destroy");
    furi_message_queue_free(state->queue);
    furi_mutex_free(state->data_mutex);
    free(state->data);
    free(state);
}

void test_tx() {
    FURI_LOG_T(TAG, "test_tx");
    FURI_LOG_D(TAG, "Creating message for address: 0x4, command: 0x11");
    InfraredMessage* message = tag_ir_create_firing_message(0x11);
    FURI_LOG_D(TAG, "Sending message (5 repeats)");
    tag_ir_send(message);
    FURI_LOG_D(TAG, "Freeing message");
    tag_ir_free_message(message);
}

void tag_app_set_log_level() {
#ifdef FURI_DEBUG
    furi_log_set_level(FuriLogLevelTrace);
#else
    furi_log_set_level(FuriLogLevelInfo);
#endif
}

int32_t tag_game_app(void* p) {
    tag_app_set_log_level();
    FURI_LOG_T(TAG, "tag_game_app");
    UNUSED(p);

    FURI_LOG_I(TAG, "Initialising app");
    tag_app_init();
    tag_ui_init(state);
    tag_ir_init(InfraredProtocolNEC, 5, 0x4);
    state->mode = TagAppModeReady;

    test_tx();

    tag_app_game_loop_run(state, 10);

    FURI_LOG_I(TAG, "Tearing down app");
    tag_ir_destroy();
    tag_ui_destroy(state);
    tag_app_destroy();
    return 0;
}
