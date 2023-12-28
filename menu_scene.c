#include "minesweeper.h"

void minesweeper_scenes_menu_on_enter(void* context) {
    UNUSED(context);
}

bool minesweeper_scenes_menu_on_event(void* context, SceneManagerEvent event) {
    furi_asssert(context);
    UNUSED(context);
    UNUSED(event);

    bool consumed = false;

    return consumed;
}

void minesweeper_scenes_menu_on_exit(void* context) {
    furi_assert(context);
    UNUSED(context);
}
