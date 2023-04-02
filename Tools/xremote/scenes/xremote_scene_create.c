#include "../xremote.h"
#include "../helpers/xremote_custom_event.h"

typedef enum {
    ButtonIndexPlus = -2,
    ButtonIndexSave = -1,
    ButtonIndexNA = 0,
} ButtonIndex;

static void xremote_create_callback(void* context, int32_t index, InputType type) {
    XRemote* app = context;
    
    uint16_t custom_type;
    if(type == InputTypePress) {
        custom_type = XRemoteCustomEventMenuVoid;
    } else if(type == InputTypeRelease) {
        custom_type = XRemoteCustomEventMenuVoid;
    } else if(type == InputTypeShort) {
        //somehow ButtonMenuItemTypeCommon uses InputTypeShort
        custom_type = XRemoteCustomEventMenuSelected;
    } else {
        furi_crash("Unexpected Input Type");
    }

    view_dispatcher_send_custom_event(app->view_dispatcher, xremote_custom_menu_event_pack(custom_type, index));
}

void xremote_scene_create_on_enter(void* context) {
    furi_assert(context);
    XRemote* app = context;
    ButtonMenu* button_menu = app->button_menu_create;
    //SceneManager* scene_manager = app->scene_manager;

    size_t item_count = cross_remote_get_item_count(app->cross_remote);
    for(size_t i = 0; i < item_count; ++i) {
        CrossRemoteItem* item = cross_remote_get_item(app->cross_remote, i);
        button_menu_add_item(
            button_menu,
            xremote_remote_item_get_name(item),
            i,
            xremote_create_callback,
            ButtonMenuItemTypeCommon,
            context);
    }

    button_menu_add_item(
        button_menu,
        "+",
        ButtonIndexPlus,
        xremote_create_callback,
        ButtonMenuItemTypeControl,
        context);

    button_menu_add_item(
        button_menu,
        "Save",
        ButtonIndexSave,
        xremote_create_callback,
        ButtonMenuItemTypeControl,
        context);

    button_menu_set_header(button_menu, "Add Cmd");
    const int16_t button_index =
        (signed)scene_manager_get_scene_state(app->scene_manager, XRemoteViewIdCreate);
    button_menu_set_selected_item(button_menu, button_index);
    scene_manager_set_scene_state(app->scene_manager, XRemoteSceneCreate, ButtonIndexNA);

    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdCreate);
}

bool xremote_scene_create_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(app->scene_manager, XRemoteSceneMenu);
        consumed = true;
        /*if(is_transmitter_idle) {
            const uint32_t possible_scenes[] = {InfraredSceneRemoteList, InfraredSceneStart};
            consumed = scene_manager_search_and_switch_to_previous_scene_one_of(
                scene_manager, possible_scenes, COUNT_OF(possible_scenes));
        } else {
            consumed = true;
        }*/
    } else if(event.type == SceneManagerEventTypeCustom) {
        const uint16_t custom_type = xremote_custom_menu_event_get_type(event.event);
        const int16_t button_index = xremote_custom_menu_event_get_value(event.event);
        if (custom_type == XRemoteCustomEventMenuSelected && button_index < 0) {
            scene_manager_set_scene_state(
                app->scene_manager, XRemoteSceneCreate, (unsigned)button_index);
            if(button_index == ButtonIndexPlus) {
                scene_manager_next_scene(app->scene_manager, XRemoteSceneCreateAdd);
                consumed = true;
            } else if(button_index == ButtonIndexSave) {
                scene_manager_next_scene(app->scene_manager, XRemoteSceneSaveRemote);
            }
        }
        /*switch(event.event) {
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
                    app->scene_manager, XRemoteSceneMenu)) {
                        scene_manager_stop(app->scene_manager);
                        view_dispatcher_stop(app->view_dispatcher);
                    }
                consumed = true;
                break;
        }*/
    }
    
    return consumed;
}

void xremote_scene_create_on_exit(void* context) {
    XRemote* app = context;
    button_menu_reset(app->button_menu_create);
}