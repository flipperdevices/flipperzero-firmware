#include "../uhf_app_i.h"
#include "../uhf_module.h"

void uhf_settings_set_module_baudrate(VariableItem* item) {
    UNUSED(item);
}

void uhf_scene_settings_on_enter(void* ctx) {
    UHFApp* uhf_app = ctx;
    VariableItem* item;
    uint8_t value_index = 0;
    M100Module* uhf_module = uhf_app->worker->module;

    item = variable_item_list_add(
        uhf_app->variable_item_list,
        "Baud Rate:",
        get_baudrate_count(),
        uhf_settings_set_module_baudrate,
        uhf_module);
    // value_index = subghz_scene_receiver_config_next_frequency(preset.frequency, subghz);
    scene_manager_set_scene_state(uhf_app->scene_manager, UHFSceneSettings, (uint32_t)item);
    variable_item_set_current_value_index(item, value_index);
    char text_buf[10] = {0};
    snprintf(text_buf, sizeof(text_buf), "%d", uhf_module->baudrate);
    variable_item_set_current_value_text(item, text_buf);
    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewMenu);
}

bool uhf_scene_settings_on_event(void* ctx, SceneManagerEvent event) {
    UHFApp* uhf_app = ctx;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UHFCustomEventSceneSettingLock) {
            scene_manager_previous_scene(uhf_app->scene_manager);
            consumed = true;
        }
    }
    return consumed;
}

void uhf_scene_settings_on_exit(void* ctx) {
    UHFApp* uhf_app = ctx;
    submenu_reset(uhf_app->submenu);
}