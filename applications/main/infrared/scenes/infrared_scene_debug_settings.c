#include "../infrared_app_i.h"

uint8_t value_index_ir;

#define DEB_PINS_COUNT (sizeof(infrared_debug_cfg_variables_text) / sizeof(char* const))
const char* const infrared_debug_cfg_variables_text[] = {
    "Internal",
    "2 (A7)",
};

static void infrared_scene_debug_settings_auto_detect_changed(VariableItem* item) {
    InfraredApp* infrared = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, value ? "ON" : "OFF");

    // enable/disable other list items
    VariableItemList* var_item_list = infrared->variable_item_list;
    variable_item_set_locked(variable_item_list_get(var_item_list, 1), value, NULL);
    variable_item_set_locked(variable_item_list_get(var_item_list, 2), value, NULL);

    infrared->last_settings->auto_detect = value;
    infrared_last_settings_save(infrared->last_settings);

    furi_hal_infrared_set_auto_detect(infrared->last_settings->auto_detect);
    if(!infrared->last_settings->auto_detect) {
        furi_hal_infrared_set_debug_out(infrared->last_settings->ext_out);
        if(infrared->last_settings->ext_5v) {
            uint8_t attempts = 0;
            while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
                furi_hal_power_enable_otg();
                furi_delay_ms(10);
            }
        } else if(furi_hal_power_is_otg_enabled()) {
            furi_hal_power_disable_otg();
        }
    } else if(furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
    }
}

static void infrared_scene_debug_settings_pin_changed(VariableItem* item) {
    InfraredApp* infrared = variable_item_get_context(item);
    value_index_ir = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, infrared_debug_cfg_variables_text[value_index_ir]);

    furi_hal_infrared_set_debug_out(value_index_ir);

    infrared->last_settings->ext_out = value_index_ir == 1;
    infrared_last_settings_save(infrared->last_settings);
}

static void infrared_scene_debug_settings_power_changed(VariableItem* item) {
    InfraredApp* infrared = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    if(value) {
        uint8_t attempts = 0;
        while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
            furi_hal_power_enable_otg();
            furi_delay_ms(10);
        }
    } else {
        if(furi_hal_power_is_otg_enabled()) {
            furi_hal_power_disable_otg();
        }
    }
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");

    infrared->last_settings->ext_5v = value;
    infrared_last_settings_save(infrared->last_settings);
}

static void infrared_debug_settings_start_var_list_enter_callback(void* context, uint32_t index) {
    InfraredApp* infrared = context;
    view_dispatcher_send_custom_event(infrared->view_dispatcher, index);
}

void infrared_scene_debug_settings_on_enter(void* context) {
    InfraredApp* infrared = context;

    VariableItemList* variable_item_list = infrared->variable_item_list;
    variable_item_list_set_enter_callback(
        variable_item_list, infrared_debug_settings_start_var_list_enter_callback, infrared);

    VariableItem* item = variable_item_list_add(
        variable_item_list,
        "Auto detect",
        2,
        infrared_scene_debug_settings_auto_detect_changed,
        infrared);

    bool auto_detect = infrared->last_settings->auto_detect;

    variable_item_set_current_value_index(item, auto_detect);
    variable_item_set_current_value_text(item, auto_detect ? "ON" : "OFF");

    item = variable_item_list_add(
        variable_item_list,
        "Send signal to",
        DEB_PINS_COUNT,
        infrared_scene_debug_settings_pin_changed,
        infrared);

    value_index_ir = infrared->last_settings->ext_out;

    variable_item_list_set_enter_callback(
        variable_item_list, infrared_debug_settings_start_var_list_enter_callback, infrared);

    variable_item_set_current_value_index(item, value_index_ir);
    variable_item_set_current_value_text(item, infrared_debug_cfg_variables_text[value_index_ir]);
    variable_item_set_locked(item, auto_detect, "Disable auto detect");

    item = variable_item_list_add(
        variable_item_list,
        "Ext Module 5v",
        2,
        infrared_scene_debug_settings_power_changed,
        infrared);
    bool enabled = infrared->last_settings->ext_5v;
    variable_item_set_current_value_index(item, enabled);
    variable_item_set_current_value_text(item, enabled ? "ON" : "OFF");
    variable_item_set_locked(item, auto_detect, "Disable auto detect");

    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewVariableItemList);
}

bool infrared_scene_debug_settings_on_event(void* context, SceneManagerEvent event) {
    InfraredApp* infrared = context;
    UNUSED(infrared);
    UNUSED(event);

    return false;
}

void infrared_scene_debug_settings_on_exit(void* context) {
    InfraredApp* infrared = context;
    variable_item_list_reset(infrared->variable_item_list);
}
