#include "../cfw_app.h"

enum VarItemListIndex {
    VarItemListIndexScreen,
    VarItemListIndexChangeDeviceName,
    VarItemListIndexChargeCap,
};

void cfw_app_scene_misc_var_item_list_callback(void* context, uint32_t index) {
    CfwApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

#define CHARGE_CAP_INTV 5
static void cfw_app_scene_misc_charge_cap_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    char cap_str[6];
    uint32_t value = (variable_item_get_current_value_index(item) + 1) * CHARGE_CAP_INTV;
    snprintf(cap_str, 6, "%lu%%", value);
    variable_item_set_current_value_text(item, cap_str);
    CFW_SETTINGS()->charge_cap = value;
    app->save_settings = true;
}

void cfw_app_scene_misc_on_enter(void* context) {
    CfwApp* app = context;
    CfwSettings* cfw_settings = CFW_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    variable_item_list_add(var_item_list, "Screen", 0, NULL, app);

    variable_item_list_add(var_item_list, "Change Device Name", 0, NULL, app);

    char cap_str[6];
    value_index = cfw_settings->charge_cap / CHARGE_CAP_INTV;
    snprintf(cap_str, 6, "%lu%%", (uint32_t)value_index * CHARGE_CAP_INTV);
    item = variable_item_list_add(
        var_item_list,
        "Charge Cap",
        100 / CHARGE_CAP_INTV,
        cfw_app_scene_misc_charge_cap_changed,
        app);
    variable_item_set_current_value_index(item, value_index - 1);
    variable_item_set_current_value_text(item, cap_str);

    variable_item_list_set_enter_callback(
        var_item_list, cfw_app_scene_misc_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, CfwAppSceneMisc));

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewVarItemList);
}

bool cfw_app_scene_misc_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, CfwAppSceneMisc, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexScreen:
            scene_manager_set_scene_state(app->scene_manager, CfwAppSceneMiscScreen, 0);
            scene_manager_next_scene(app->scene_manager, CfwAppSceneMiscScreen);
            break;
        case VarItemListIndexChangeDeviceName:
            scene_manager_set_scene_state(app->scene_manager, CfwAppSceneMiscRename, 0);
            scene_manager_next_scene(app->scene_manager, CfwAppSceneMiscRename);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_misc_on_exit(void* context) {
    CfwApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
