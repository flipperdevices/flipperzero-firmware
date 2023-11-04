#include "../xremote.h"

enum SubmenuIndexEdit {
    SubmenuIndexRename = 10,
    SubmenuIndexEdit,
    SubmenuIndexDelete,
};

void xremote_scene_xr_list_edit_item_submenu_callback(void* context, uint32_t index) {
    XRemote* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void xremote_scene_xr_list_edit_item_on_enter(void* context) {
    XRemote* app = context;
    //submenu_add_item(app->editmenu, "Rename", SubmenuIndexRename, xremote_scene_xr_list_edit_item_submenu_callback, app);
    submenu_add_item(
        app->editmenu,
        "Edit",
        SubmenuIndexEdit,
        xremote_scene_xr_list_edit_item_submenu_callback,
        app);
    submenu_add_item(
        app->editmenu,
        "Delete",
        SubmenuIndexDelete,
        xremote_scene_xr_list_edit_item_submenu_callback,
        app);

    submenu_set_selected_item(
        app->editmenu, scene_manager_get_scene_state(app->scene_manager, XRemoteSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdEditItem);
}

bool xremote_scene_xr_list_edit_item_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_previous_scene(app->scene_manager);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexDelete) {
            cross_remote_delete(app->cross_remote);
        } else if(event.event == SubmenuIndexEdit) {
            strncpy(
                app->text_store[0],
                cross_remote_get_name(app->cross_remote),
                XREMOTE_MAX_REMOTE_NAME_LENGTH);
            scene_manager_next_scene(app->scene_manager, XRemoteSceneCreate);
            return 0;
        } else if(event.event == SubmenuIndexRename) {
            //scene_manager_next_scene(app->scene_manager, XRemoteSceneSaveRemoteItem);
            //scene_manager_next_scene(app->scene_manager, XRemoteSceneWip);
            return 0;
        }
        scene_manager_previous_scene(app->scene_manager);
    }
    return 0;
}

void xremote_scene_xr_list_edit_item_on_exit(void* context) {
    XRemote* app = context;
    submenu_reset(app->editmenu);
}