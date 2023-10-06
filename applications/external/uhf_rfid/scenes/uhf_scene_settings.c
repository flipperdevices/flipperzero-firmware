#include "../uhf_app_i.h"
#include "../uhf_module.h"

void uhf_settings_set_module_baudrate(VariableItem* item) {
    M100Module* uhf_module = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    if(index >= BAUD_RATES_COUNT) {
        return;
    }
    uint32_t baudrate = BAUD_RATES[index];
    m100_set_baudrate(uhf_module, baudrate);
    char text_buf[10];
    snprintf(text_buf, sizeof(text_buf), "%lu", uhf_module->baudrate);
    variable_item_set_current_value_text(item, text_buf);
}

void uhf_settings_set_module_powerdb(VariableItem* item) {
    M100Module* uhf_module = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    if(index >= POWER_DBM_COUNT) {
        return;
    }
    uint16_t power = POWER_DBM[index];
    m100_set_transmitting_power(uhf_module, power);
    char text_buf[10];
    snprintf(text_buf, sizeof(text_buf), "%ddBm", uhf_module->transmitting_power);
    variable_item_set_current_value_text(item, text_buf);
}

void uhf_settings_set_module_working_region(VariableItem* item) {
    M100Module* uhf_module = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    if(index >= WORKING_REGIONS_COUNT) {
        return;
    }
    WorkingRegion region = WORKING_REGIONS[index];
    m100_set_working_region(uhf_module, region);
    variable_item_set_current_value_text(item, WORKING_REGIONS_STR[index]);
}

uint8_t uhf_settings_get_module_baudrate_index(M100Module* module) {
    for(uint8_t i = 0; i < BAUD_RATES_COUNT; i++) {
        if(BAUD_RATES[i] == module->baudrate) {
            return i;
        }
    }
    return 0;
}

uint8_t uhf_settings_get_module_power_index(M100Module* module) {
    for(uint8_t i = 0; i < BAUD_RATES_COUNT; i++) {
        if(POWER_DBM[i] == module->transmitting_power) {
            return i;
        }
    }
    return 0;
}

uint8_t uhf_settings_get_module_working_region_index(M100Module* module) {
    for(uint8_t i = 0; i < WORKING_REGIONS_COUNT; i++) {
        if(WORKING_REGIONS[i] == module->region) {
            return i;
        }
    }
    return 0;
}

void uhf_scene_settings_on_enter(void* ctx) {
    UHFApp* uhf_app = ctx;
    M100Module* uhf_module = uhf_app->worker->module;
    VariableItem* item;
    VariableItemList* variable_item_list = uhf_app->variable_item_list;

    uint8_t value_index = uhf_settings_get_module_baudrate_index(uhf_module);
    char text_buf[10];
    snprintf(text_buf, sizeof(text_buf), "%lu", uhf_module->baudrate);
    item = variable_item_list_add(
        variable_item_list,
        "Baudrate:",
        BAUD_RATES_COUNT,
        uhf_settings_set_module_baudrate,
        uhf_module);

    variable_item_set_current_value_text(item, text_buf);
    variable_item_set_current_value_index(item, value_index);

    value_index = uhf_settings_get_module_power_index(uhf_module);
    item = variable_item_list_add(
        variable_item_list,
        "Power(DBM):",
        POWER_DBM_COUNT,
        uhf_settings_set_module_powerdb,
        uhf_module);
    snprintf(text_buf, sizeof(text_buf), "%ddBm", uhf_module->transmitting_power);
    variable_item_set_current_value_text(item, text_buf);
    variable_item_set_current_value_index(item, value_index);

    value_index = uhf_settings_get_module_working_region_index(uhf_module);
    item = variable_item_list_add(
        variable_item_list,
        "Region:",
        WORKING_REGIONS_COUNT,
        uhf_settings_set_module_working_region,
        uhf_module);
    variable_item_set_current_value_text(item, WORKING_REGIONS_STR[value_index]);
    variable_item_set_current_value_index(item, value_index);

    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewVariableItemList);
}

bool uhf_scene_settings_on_event(void* ctx, SceneManagerEvent event) {
    UHFApp* uhf_app = ctx;
    UNUSED(uhf_app);
    UNUSED(event);
    return false;
}

void uhf_scene_settings_on_exit(void* ctx) {
    UHFApp* uhf_app = ctx;
    variable_item_list_set_selected_item(uhf_app->variable_item_list, 0);
    variable_item_list_reset(uhf_app->variable_item_list);
}