#include "../xremote.h"
#include "../views/xremote_pause_set.h"

void xremote_scene_pause_set_on_enter(void* context) {
    furi_assert(context);
    XRemote* app = context;
    UNUSED(app);
}

bool xremote_scene_pause_set_on_event(void* context, SceneManagerEvent* event) {
    XRemote* app = context;

    UNUSED(app);
    UNUSED(event);
    bool consumed = false;
    return consumed;
}


void xremote_scene_pause_set_on_exit(void* context) {
    UNUSED(context);
}
