#include "../xremote.h"
#include "../models/cross/xremote_remote.h"

#include <string.h>
#include <toolbox/path.h>

void xremote_scene_save_remote_item_on_enter(void* context) {
    XRemote* app = context;
    TextInput* text_input = app->text_input;

    text_input_set_header_text(text_input, "Name the Sequence");

    size_t enter_name_length = XREMOTE_MAX_REMOTE_NAME_LENGTH;
    CrossRemoteItem* item = cross_remote_get_item(app->cross_remote, app->edit_item);
    strncpy(app->text_store[0], furi_string_get_cstr(item->name), enter_name_length);
    text_input_set_result_callback(
        text_input,
        xremote_text_input_callback,
        context,
        app->text_store[0],
        enter_name_length,
        false);

    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdTextInput);
}

bool xremote_scene_save_remote_item_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    CrossRemote* remote = app->cross_remote;
    SceneManager* scene_manager = app->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        cross_remote_rename_item(remote, app->edit_item, app->text_store[0]);
        scene_manager_next_scene(scene_manager, XRemoteSceneCreate);
        consumed = true;
    }
    return consumed;
}

void xremote_scene_save_remote_item_on_exit(void* context) {
    XRemote* app = context;
    size_t enter_name_length = XREMOTE_MAX_REMOTE_NAME_LENGTH;
    strncpy(app->text_store[0], "", enter_name_length);
}