#include "../tpms_app_i.h"

#define TAG "TPMSRelearn"

enum TPMSRelearnSetting {
    TPMSRelearnSettingOff,
    TPMSRelearnSettingOn,
    TPMSRelearnSettingNum,
};

const char* const relearn_setting_text[TPMSRelearnSettingNum] = {
    "OFF",
    "ON",
};

enum TPMSRelearnType {
    TPMSRelearnCommon, // Just signal without data
    // TPMSRelearnGM_FORD,
    // TPMSRelearnAnotherOEM,
    TPMSRelearnTypeNum,
};

const char* const relearn_type_text[TPMSRelearnTypeNum] = {
    "Common",
};

static void tpms_scene_relearn_setting_callback(VariableItem* item) {
    TPMSApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    if(index == TPMSRelearnSettingOn) {
        app->relearn = TPMSRelearnOn;
    } else {
        app->relearn = TPMSRelearnOff;
    }
    variable_item_set_current_value_text(item, relearn_setting_text[index]);
}

static void tpms_scene_relearn_type_callback(VariableItem* item) {
    TPMSApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    if(index == TPMSRelearnCommon) {
        app->relearn_type = TPMSRelearnTypeCommon;
    } else {
        FURI_LOG_E(TAG, "Relearn type %d not implemented", index);
        return;
    }
    variable_item_set_current_value_text(item, relearn_type_text[index]);
}

void tpms_scene_relearn_config_on_enter(void* context) {
    TPMSApp* app = context;
    VariableItemList* var_item_list = app->variable_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Relearn 125kHz",
        TPMSRelearnSettingNum,
        tpms_scene_relearn_setting_callback,
        app);
    // scene_manager_set_scene_state(app->scene_manager, TPMSSceneReceiverConfig, (uint32_t)item);
    variable_item_set_current_value_index(item, app->relearn);
    variable_item_set_current_value_text(item, relearn_setting_text[app->relearn]);

    item = variable_item_list_add(
        var_item_list, "Type", TPMSRelearnTypeNum, tpms_scene_relearn_type_callback, app);
    variable_item_set_current_value_index(item, app->relearn_type);
    variable_item_set_current_value_text(item, relearn_type_text[app->relearn_type]);

    view_dispatcher_switch_to_view(app->view_dispatcher, TPMSViewVariableItemList);
}
bool tpms_scene_relearn_config_on_event(void* context, SceneManagerEvent event) {
    TPMSApp* app = context;
    UNUSED(app);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
    }
    return consumed;
}

void tpms_scene_relearn_config_on_exit(void* context) {
    TPMSApp* app = context;
    variable_item_list_set_selected_item(app->variable_item_list, 0);
    variable_item_list_reset(app->variable_item_list);
}
