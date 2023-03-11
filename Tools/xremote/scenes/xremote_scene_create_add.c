#include "../xremote.h"
#include "../helpers/xremote_custom_event.h"

typedef enum {
    ButtonIndexIr = -2,
    ButtonIndexSubghz = -1,
    ButtonIndexPause = 0,
} ButtonIndex;

/*void xremote_create_callback(XRemoteCustomEvent event, void* context) {
    furi_assert(context);
    XRemote* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}*/

static void xremote_create_add_callback(void* context, int32_t index, InputType type) {
    XRemote* app = context;
    UNUSED(app);
    UNUSED(index);
    UNUSED(type);

    //view_dispatcher_send_custom_event(app->view_dispatcher, ??);
}

void xremote_scene_create_add_on_enter(void* context) {
    furi_assert(context);
    XRemote* app = context;
    ButtonMenu* button_menu = app->button_menu;
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

    //xremote_create_set_callback(app->xremote_create, xremote_create_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdCreateAdd);
}

bool xremote_scene_create_add_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
            case XRemoteCustomEventCreateLeft:
            case XRemoteCustomEventCreateRight:
                break;
            case XRemoteCustomEventCreateUp:
            case XRemoteCustomEventCreateDown:
                break;
            case XRemoteCustomEventCreateBack:
                notification_message(app->notification, &sequence_reset_red);
                notification_message(app->notification, &sequence_reset_green);
                notification_message(app->notification, &sequence_reset_blue);
                if(!scene_manager_search_and_switch_to_previous_scene(
                    app->scene_manager, XRemoteSceneCreate)) {
                        scene_manager_stop(app->scene_manager);
                        view_dispatcher_stop(app->view_dispatcher);
                    }
                consumed = true;
                break;
        }
    }
    
    return consumed;
}

void xremote_scene_create_add_on_exit(void* context) {
    XRemote* app = context;
    button_menu_reset(app->button_menu);
}