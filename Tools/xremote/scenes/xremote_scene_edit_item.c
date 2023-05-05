#include "../xremote.h"

enum SubmenuIndexEdit {
    SubmenuIndexRename = 10,
    SubmenuIndexDelete,
};

void xremote_scene_edit_item_submenu_callback(void* context, uint32_t index) {
    XRemote* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void xremote_scene_edit_item_on_enter(void* context)
{
    XRemote* app = context;
    submenu_add_item(app->editmenu, "Rename", SubmenuIndexRename, xremote_scene_edit_item_submenu_callback, app);
    submenu_add_item(app->editmenu, "Delete", SubmenuIndexDelete, xremote_scene_edit_item_submenu_callback, app);

    submenu_set_selected_item(app->editmenu, scene_manager_get_scene_state(app->scene_manager, XRemoteSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdEditItem);
}

bool xremote_scene_edit_item_on_event(void* context, SceneManagerEvent event)
{
    UNUSED(context);
    UNUSED(event);
    return 0;
}

void xremote_scene_edit_item_on_exit(void* context)
{
    UNUSED(context);
}