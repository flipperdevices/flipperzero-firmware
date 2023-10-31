#include "../evil_portal_app_i.h"
#include "../helpers/evil_portal_storage.h"

void evil_portal_text_input_callback(void* context) {
    furi_assert(context);
    Evil_PortalApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, Evil_PortalEventTextInput);
}

void evil_portal_scene_rename_on_enter(void* context) {
    Evil_PortalApp* app = context;
    TextInput* text_input = app->text_input;
    evil_portal_read_ap_name(app);
    text_input_set_header_text(text_input, "AP Name/SSID");
    text_input_set_result_callback(
        text_input,
        evil_portal_text_input_callback,
        context,
        app->ap_name,
        sizeof(app->ap_name),
        false);
    view_dispatcher_switch_to_view(app->view_dispatcher, Evil_PortalAppViewTextInput);
}

bool evil_portal_scene_rename_on_event(void* context, SceneManagerEvent event) {
    Evil_PortalApp* app = context;
    SceneManager* scene_manager = app->scene_manager;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        evil_portal_write_ap_name(app);
        scene_manager_search_and_switch_to_previous_scene(scene_manager, Evil_PortalSceneStart);
        consumed = true;
    }
    return consumed;
}

void evil_portal_scene_rename_on_exit(void* context) {
    Evil_PortalApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
