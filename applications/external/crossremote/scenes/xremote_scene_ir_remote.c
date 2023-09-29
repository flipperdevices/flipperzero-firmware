#include "../xremote.h"

typedef enum {
    ButtonIndexPlus = -2,
    ButtonIndexEdit = -1,
    ButtonIndexNA = 0,
} ButtonIndex;

static void
    xremote_scene_ir_remote_button_menu_callback(void* context, int32_t index, InputType type) {
    XRemote* app = context;

    uint16_t custom_type;
    if(type == InputTypePress) {
        custom_type = XRemoteCustomEventMenuVoid;
    } else if(type == InputTypeRelease) {
        custom_type = XRemoteCustomEventMenuAddIrSelected;
    } else if(type == InputTypeShort) {
        custom_type = XRemoteCustomEventMenuVoid;
    } else {
        furi_crash("Unexpected IR input type");
    }

    view_dispatcher_send_custom_event(
        app->view_dispatcher, xremote_custom_menu_event_pack(custom_type, index));
}

void xremote_scene_ir_remote_on_enter(void* context) {
    XRemote* app = context;
    ButtonMenu* button_menu = app->button_menu_ir;

    size_t button_count = xremote_ir_remote_get_button_count(app->ir_remote_buffer);
    for(size_t i = 0; i < button_count; i++) {
        InfraredRemoteButton* button = xremote_ir_remote_get_button(app->ir_remote_buffer, i);
        button_menu_add_item(
            button_menu,
            xremote_ir_remote_button_get_name(button),
            i,
            xremote_scene_ir_remote_button_menu_callback,
            ButtonMenuItemTypeCommon,
            context);
    }

    button_menu_set_header(button_menu, "Select Cmd");
    const int16_t button_index =
        (signed)scene_manager_get_scene_state(app->scene_manager, XRemoteViewIdIrRemote);
    button_menu_set_selected_item(button_menu, button_index);

    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdIrRemote);
}

bool xremote_scene_ir_remote_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        const uint16_t custom_type = xremote_custom_menu_event_get_type(event.event);
        const int16_t button_index = xremote_custom_menu_event_get_value(event.event);
        if(custom_type == XRemoteCustomEventMenuAddIrSelected) {
            scene_manager_set_scene_state(
                app->scene_manager, XRemoteSceneIrRemote, (unsigned)button_index);

            InfraredRemoteButton* ir_button =
                xremote_ir_remote_get_button(app->ir_remote_buffer, button_index);
            const char* button_name = xremote_ir_remote_button_get_name(ir_button);
            InfraredSignal* signal = xremote_ir_remote_button_get_signal(ir_button);

            cross_remote_add_ir_item(app->cross_remote, button_name, signal);
            scene_manager_next_scene(app->scene_manager, XRemoteSceneCreate);
            consumed = true;
        }
    }

    return consumed;
}

void xremote_scene_ir_remote_on_exit(void* context) {
    XRemote* app = context;
    button_menu_reset(app->button_menu_ir);
}