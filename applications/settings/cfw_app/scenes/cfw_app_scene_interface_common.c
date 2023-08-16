#include "../cfw_app.h"

enum VarItemListIndex {
    VarItemListIndexSortDirsFirst,
};

void cfw_app_scene_interface_common_var_item_list_callback(void* context, uint32_t index) {
    CfwApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void cfw_app_scene_interface_common_sort_dirs_first_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    CFW_SETTINGS()->sort_dirs_first = value;
    app->save_settings = true;
}

void cfw_app_scene_interface_common_on_enter(void* context) {
    CfwApp* app = context;
    CfwSettings* cfw_settings = CFW_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Sort Dirs First",
        2,
        cfw_app_scene_interface_common_sort_dirs_first_changed,
        app);
    variable_item_set_current_value_index(item, cfw_settings->sort_dirs_first);
    variable_item_set_current_value_text(item, cfw_settings->sort_dirs_first ? "ON" : "OFF");

    variable_item_list_set_enter_callback(
        var_item_list, cfw_app_scene_interface_common_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, CfwAppSceneInterfaceCommon));

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewVarItemList);
}

bool cfw_app_scene_interface_common_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, CfwAppSceneInterfaceCommon, event.event);
        consumed = true;
        switch(event.event) {
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_interface_common_on_exit(void* context) {
    CfwApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
