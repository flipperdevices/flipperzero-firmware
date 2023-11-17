#include "../xremote.h"
#include "../helpers/xremote_custom_event.h"

typedef enum {
    ButtonIndexIr = -3,
    ButtonIndexSubghz = -2,
    ButtonIndexPause = -1,
    ButtonIndexNA = 0,
} ButtonIndex;

static void xremote_create_add_callback(void* context, int32_t index, InputType type) {
    XRemote* app = context;

    uint16_t custom_type;
    if(type == InputTypePress) {
        custom_type = XRemoteCustomEventMenuVoid;
    } else if(type == InputTypeRelease) {
        custom_type = XRemoteCustomEventMenuAddSelected;
    } else if(type == InputTypeShort) {
        custom_type = XRemoteCustomEventMenuVoid;
    } else {
        furi_crash("Unexpected Input Type");
    }

    view_dispatcher_send_custom_event(
        app->view_dispatcher, xremote_custom_menu_event_pack(custom_type, index));
}

void xremote_scene_create_add_on_enter(void* context) {
    furi_assert(context);
    XRemote* app = context;
    ButtonMenu* button_menu = app->button_menu_create_add;
    SceneManager* scene_manager = app->scene_manager;

    button_menu_add_item(
        button_menu,
        "Infrared",
        ButtonIndexIr,
        xremote_create_add_callback,
        ButtonMenuItemTypeCommon,
        context);

    button_menu_add_item(
        button_menu,
        "SubGhz",
        ButtonIndexSubghz,
        xremote_create_add_callback,
        ButtonMenuItemTypeCommon,
        context);

    button_menu_add_item(
        button_menu,
        "Pause",
        ButtonIndexPause,
        xremote_create_add_callback,
        ButtonMenuItemTypeCommon,
        context);

    button_menu_set_header(button_menu, "Choose Type");
    const int16_t button_index =
        (signed)scene_manager_get_scene_state(scene_manager, XRemoteViewIdCreateAdd);
    button_menu_set_selected_item(button_menu, button_index);

    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdCreateAdd);
}

bool xremote_scene_create_add_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        const uint16_t custom_type = xremote_custom_menu_event_get_type(event.event);
        const int16_t button_index = xremote_custom_menu_event_get_value(event.event);
        if(custom_type == XRemoteCustomEventMenuAddSelected) {
            furi_assert(button_index < 0);
            scene_manager_set_scene_state(
                app->scene_manager, XRemoteSceneCreate, (unsigned)button_index);
            if(button_index == ButtonIndexIr) {
                scene_manager_next_scene(app->scene_manager, XRemoteSceneIrList);
            }
            if(button_index == ButtonIndexSubghz) {
                //scene_manager_next_scene(app->scene_manager, XRemoteSceneSgList);
                scene_manager_next_scene(app->scene_manager, XRemoteSceneWip);
            }
            if(button_index == ButtonIndexPause) {
                scene_manager_next_scene(app->scene_manager, XRemoteScenePauseSet);
            }
            consumed = true;
        }
    }

    return consumed;
}

void xremote_scene_create_add_on_exit(void* context) {
    XRemote* app = context;
    button_menu_reset(app->button_menu_create_add);
}