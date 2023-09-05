// #include "../uhf_app_i.h"

// void uhf_settings_set_module_baudrate(VariableItem* item) {
//     uint32_t baudrate =
// }

// void uhf_scene_on_enter(void* ctx) {
//     UHFApp* uhf_app = ctx;
//     view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewMenu);
// }

// bool uhf_scene_on_event(void* ctx, SceneManagerEvent event) {
//     UHFApp* uhf_app = ctx;
//     bool consumed = false;
//     VariableItem* item;
//     uint8_t value_index;
//     // SubGhzSetting* setting = subghz_txrx_get_setting(subghz->txrx);
//     // SubGhzRadioPreset preset = subghz_txrx_get_preset(subghz->txrx);

//     item = variable_item_list_add(
//         subghz->variable_item_list,
//         "Baud Rate:",
//         subghz_setting_get_frequency_count(setting),
//         subghz_scene_receiver_config_set_frequency,
//         subghz);
//     value_index = subghz_scene_receiver_config_next_frequency(preset.frequency, subghz);
//     scene_manager_set_scene_state(
//         subghz->scene_manager, SubGhzSceneReceiverConfig, (uint32_t)item);
//     variable_item_set_current_value_index(item, value_index);
//     char text_buf[10] = {0};
//     uint32_t frequency = subghz_setting_get_frequency(setting, value_index);
//     snprintf(
//         text_buf,
//         sizeof(text_buf),
//         "%lu.%02lu",
//         frequency / 1000000,
//         (frequency % 1000000) / 10000);
//     variable_item_set_current_value_text(item, text_buf);
// }

// void uhf_scene_on_exit(void* ctx) {
//     UHFApp* uhf_app = ctx;
//     submenu_reset(uhf_app->submenu);
// }