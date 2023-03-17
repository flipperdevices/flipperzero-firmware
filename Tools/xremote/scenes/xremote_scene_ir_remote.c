#include "../xremote.h"

typedef enum {
    ButtonIndexPlus = -2,
    ButtonIndexEdit = -1,
    ButtonIndexNA = 0,
} ButtonIndex;

void xremote_scene_ir_remote_on_enter(void* context) {
    UNUSED(context);
}

bool xremote_scene_ir_remote_on_event(void* context, SceneManagerEvent event) {
    bool consumed = false;
    UNUSED(context);
    UNUSED(event);
    
    return consumed;
}

void xremote_scene_ir_remote_on_exit(void* context) {
    XRemote* app = context;
    button_menu_reset(app->button_menu);
}