#include "../uhf_app_i.h"
#include "../uhf_module.h"

void uhf_settings_set_module_baudrate(VariableItem* item) {
    M100Module* uhf_module = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    uint32_t baudrate = BAUD_RATES[index];
    m100_set_baudrate(uhf_module, baudrate);
    char text_buf[10];
    snprintf(text_buf, sizeof(text_buf), "%lu", uhf_module->baudrate);
    variable_item_set_current_value_text(item, text_buf);
}

uint8_t uhf_settings_get_module_baudrate_index(M100Module* module) {
    uint32_t baudrate = module->baudrate;
    for(uint8_t i = 0; i < sizeof(BAUD_RATES); i++) {
        if(BAUD_RATES[i] == baudrate) {
            return i;
        }
    }
    return 0;
}

uint8_t uhf_settings_get_module_power_index(M100Module* module) {
    uint16_t power = module->transmitting_power;
    for(uint8_t i = 0; i < sizeof(POWER_DBM); i++) {
        if(POWER_DBM[i] == power) {
            return i;
        }
    }
    return 0;
}

void uhf_scene_settings_on_enter(void* ctx) {
    furi_assert(ctx != NULL, "ctx is NULL in uhf_scene_settings_on_enter");
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
        sizeof(BAUD_RATES),
        uhf_settings_set_module_baudrate,
        uhf_module);

    variable_item_set_current_value_text(item, text_buf);
    variable_item_set_current_value_index(item, value_index);

    item = variable_item_list_add(
        variable_item_list, "Power(DBM):", sizeof(POWER_DBM), m100_set_power, uhf_module);

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