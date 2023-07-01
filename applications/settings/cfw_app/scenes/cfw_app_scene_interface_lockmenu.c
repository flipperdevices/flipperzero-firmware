#include "../cfw_app.h"

enum VarItemListIndex {
    VarItemListIndexMenuType,
};

void cfw_app_scene_interface_lockmenu_var_item_list_callback(void* context, uint32_t index) {
    CfwApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void cfw_app_scene_interface_lockmenu_type_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "Adv Grid" : "List");
    CFW_SETTINGS()->lock_menu_type = value;
    app->save_settings = true;
}

void cfw_app_scene_interface_lockmenu_on_enter(void* context) {
    CfwApp* app = context;
    CfwSettings* cfw_settings = CFW_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list, "Style", 2, cfw_app_scene_interface_lockmenu_type_changed, app);
    variable_item_set_current_value_index(item, cfw_settings->lock_menu_type);
    variable_item_set_current_value_text(item, cfw_settings->lock_menu_type ? "Adv Grid" : "List");

    variable_item_list_set_enter_callback(
        var_item_list, cfw_app_scene_interface_lockmenu_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, CfwAppSceneInterfaceLockmenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewVarItemList);
}

bool cfw_app_scene_interface_lockmenu_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, CfwAppSceneInterfaceLockmenu, event.event);
        consumed = true;
        switch(event.event) {
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_interface_lockmenu_on_exit(void* context) {
    CfwApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
