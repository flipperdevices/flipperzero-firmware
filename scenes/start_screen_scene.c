#include "../minesweeper.h"
#include "../views/start_screen.h"

void minesweeper_scene_start_screen_on_enter(void* context) {
    furi_assert(context);
    UNUSED(context);
}

bool minesweeper_scene_start_screen_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);

    App* app = context;
    bool consumed = false;

    UNUSED(app);

    if (event.type == SceneManagerEventTypeCustom) {
        // Check custom event enums
        consumed = true;
    }

    return consumed;
}

void minesweeper_scene_start_screen_on_exit(void* context) {
    furi_assert(context);
    UNUSED(context);
}
