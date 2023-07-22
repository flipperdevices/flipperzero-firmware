#include "../cfw_app.h"

enum VarItemListIndex {
    VarItemListIndexSubghzFrequencies,
    VarItemListIndexSubghzExtend,
    VarItemListIndexSubghzBypass,
};

void cfw_app_scene_protocols_var_item_list_callback(void* context, uint32_t index) {
    CfwApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void cfw_app_scene_protocols_subghz_extend_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    app->subghz_extend = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, app->subghz_extend ? "ON" : "OFF");
    app->save_subghz = true;
}

static void cfw_app_scene_protocols_subghz_bypass_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    app->subghz_bypass = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, app->subghz_bypass ? "ON" : "OFF");
    app->save_subghz = true;
}

void cfw_app_scene_protocols_on_enter(void* context) {
    CfwApp* app = context;
    // CfwSettings* cfw_settings = CFW_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    variable_item_list_add(var_item_list, "SubGHz Frequencies", 0, NULL, app);

    item = variable_item_list_add(
        var_item_list, "SubGHz Extend", 2, cfw_app_scene_protocols_subghz_extend_changed, app);
    variable_item_set_current_value_index(item, app->subghz_extend);
    variable_item_set_current_value_text(item, app->subghz_extend ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list, "SubGHz Bypass", 2, cfw_app_scene_protocols_subghz_bypass_changed, app);
    variable_item_set_current_value_index(item, app->subghz_bypass);
    variable_item_set_current_value_text(item, app->subghz_bypass ? "ON" : "OFF");

    variable_item_list_set_enter_callback(
        var_item_list, cfw_app_scene_protocols_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, CfwAppSceneProtocols));

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewVarItemList);
}

bool cfw_app_scene_protocols_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, CfwAppSceneProtocols, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexSubghzFrequencies:
            scene_manager_next_scene(app->scene_manager, CfwAppSceneProtocolsFrequencies);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_protocols_on_exit(void* context) {
    CfwApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
