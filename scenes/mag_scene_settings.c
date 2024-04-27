#include "../mag_i.h"

#define TAG "MagSceneEmulateConfig"

#define GPIO_COUNT 8
static const char* gpio[GPIO_COUNT] = {
    [MagSettingPinA7] = "2 (A7)",
    [MagSettingPinA6] = "3 (A6)",
    [MagSettingPinA4] = "4 (A4)",
    [MagSettingPinB3] = "5 (B3)",
    [MagSettingPinB2] = "6 (B2)",
    [MagSettingPinC3] = "7 (C3)",
    [MagSettingPinC1] = "15 (C1)",
    [MagSettingPinC0] = "16 (C0)",
};

static void mag_scene_settings_set_gpio(VariableItem* item, MagSettingPin* pin_to_set) {
    MagSettingPin pin = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, gpio[pin]);
    *pin_to_set = pin;
}

static void mag_scene_settings_set_gpio_input(VariableItem* item) {
    Mag* mag = variable_item_get_context(item);
    mag_scene_settings_set_gpio(item, &mag->setting->pin_input);
};

static void mag_scene_settings_set_gpio_output(VariableItem* item) {
    Mag* mag = variable_item_get_context(item);
    mag_scene_settings_set_gpio(item, &mag->setting->pin_output);
};

static void mag_scene_settings_set_gpio_enable(VariableItem* item) {
    Mag* mag = variable_item_get_context(item);
    mag_scene_settings_set_gpio(item, &mag->setting->pin_enable);
};

static void mag_pin_variable_item_list_add(
    Mag* mag,
    const char* label,
    MagSettingPin pin,
    VariableItemChangeCallback change_callback) {
    VariableItem* item =
        variable_item_list_add(mag->variable_item_list, label, GPIO_COUNT, change_callback, mag);
    variable_item_set_current_value_index(item, pin);
    variable_item_set_current_value_text(item, gpio[pin]);
}
// static const char* uart_pins[] = {[DapUartTypeUSART1] = "13,14", [DapUartTypeLPUART1] = "15,16"};
// static const char* uart_swap[] = {[DapUartTXRXNormal] = "No", [DapUartTXRXSwap] = "Yes"};

void mag_scene_settings_on_enter(void* context) {
    Mag* mag = context;

    mag_pin_variable_item_list_add(
        mag, "Input pin:", mag->setting->pin_input, mag_scene_settings_set_gpio_input);
    mag_pin_variable_item_list_add(
        mag, "Output pin:", mag->setting->pin_output, mag_scene_settings_set_gpio_output);
    mag_pin_variable_item_list_add(
        mag, "Enable pin:", mag->setting->pin_enable, mag_scene_settings_set_gpio_enable);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewVariableItemList);
}

bool mag_scene_settings_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    SceneManager* scene_manager = mag->scene_manager;
    bool consumed = false;

    UNUSED(mag);
    UNUSED(scene_manager);
    UNUSED(event);

    return consumed;
}

void mag_scene_settings_on_exit(void* context) {
    Mag* mag = context;
    variable_item_list_set_selected_item(mag->variable_item_list, 0);
    variable_item_list_reset(mag->variable_item_list);
}