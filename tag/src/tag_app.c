#include "tag_app.h"

char* TAG = "tag";

static TagAppState* state;

TagAppState* tag_app_state_get() {
    return state;
}

void tag_app_init() {
    state = malloc(sizeof(state));
    state->app_mode = AppUninitialised;
}

void tag_app_start_playing() {
    // TODO
}

void tag_app_stop_playing() {
    // TODO - stop playing
}

int32_t tag_game_app(void* p) {
    UNUSED(p);
    FURI_LOG_I("TEST", "Hello world");
    FURI_LOG_I("TEST", "I'm tag_game!");
    tag_app_init();

    return 0;
}
