#include "../xremote.h"

typedef enum {
    ButtonIndexPlus = -2,
    ButtonIndexEdit = -1,
    ButtonIndexNA = 0,
} ButtonIndex;

static void xremote_scene_ir_remote_button_menu_callback(void* context, int32_t index, InputType type) {
    XRemote* app = context;

    UNUSED(app);
    UNUSED(index);
    UNUSED(type);
}

void xremote_scene_ir_remote_on_enter(void* context) {
    XRemote* app = context;

    size_t button_count = xremote_ir_remote_get_button_count(app->ir_remote_buffer);
    for(size_t i = 0; i < button_count; i++) {
        InfraredRemoteButton* button = xremote_ir_remote_get_button(app->ir_remote_buffer, i);
        button_menu_add_item(
            app->button_menu,
            xremote_ir_remote_button_get_name(button),
            i,
            xremote_scene_ir_remote_button_menu_callback,
            ButtonMenuItemTypeCommon,
            context);
    }

    button_menu_add_item(
        app->button_menu,
        "Work in Progress",
        ButtonIndexPlus,
        xremote_scene_ir_remote_button_menu_callback,
        ButtonMenuItemTypeCommon,
        context);

    button_menu_set_header(app->button_menu, "Select Cmd");
    const int16_t button_index = 
        (signed)scene_manager_get_scene_state(app->scene_manager, XRemoteViewIdIrRemote);
    button_menu_set_selected_item(app->button_menu, button_index);

    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdIrRemote);
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