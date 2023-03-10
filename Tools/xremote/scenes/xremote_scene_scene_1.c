#include "../xremote.h"
#include "../helpers/xremote_custom_event.h"
#include "../views/xremote_scene_1.h"

typedef enum {
    ButtonIndexPlus = -2,
    ButtonIndexEdit = -1,
    ButtonIndexNA = 0,
} ButtonIndex;

/*void xremote_scene_1_callback(XRemoteCustomEvent event, void* context) {
    furi_assert(context);
    XRemote* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}*/

static void xremote_scene_1_callback(void* context, int32_t index, InputType type) {
    XRemote* app = context;
    UNUSED(app);
    UNUSED(index);
    UNUSED(type);

    //view_dispatcher_send_custom_event(app->view_dispatcher, ??);
}

void xremote_scene_scene_1_on_enter(void* context) {
    furi_assert(context);
    XRemote* app = context;
    ButtonMenu* button_menu = app->button_menu;
    SceneManager* scene_manager = app->scene_manager;

    button_menu_add_item(
        button_menu,
        "+",
        ButtonIndexPlus,
        xremote_scene_1_callback,
        ButtonMenuItemTypeControl,
        context);

    button_menu_set_header(button_menu, "Random Name");
    const int16_t button_index =
        (signed)scene_manager_get_scene_state(scene_manager, XRemoteViewIdScene1);
    button_menu_set_selected_item(button_menu, button_index);

    //xremote_scene_1_set_callback(app->xremote_scene_1, xremote_scene_1_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdScene1);
}

bool xremote_scene_scene_1_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
            case XRemoteCustomEventScene1Left:
            case XRemoteCustomEventScene1Right:
                break;
            case XRemoteCustomEventScene1Up:
            case XRemoteCustomEventScene1Down:
                break;
            case XRemoteCustomEventScene1Back:
                notification_message(app->notification, &sequence_reset_red);
                notification_message(app->notification, &sequence_reset_green);
                notification_message(app->notification, &sequence_reset_blue);
                if(!scene_manager_search_and_switch_to_previous_scene(
                    app->scene_manager, XRemoteSceneMenu)) {
                        scene_manager_stop(app->scene_manager);
                        view_dispatcher_stop(app->view_dispatcher);
                    }
                consumed = true;
                break;
        }
    }
    
    return consumed;
}

void xremote_scene_scene_1_on_exit(void* context) {
    XRemote* app = context;
    UNUSED(app);
}