#include "../mag_i.h"
#include "../mag_state.h"
#include "../helpers/mag_helpers.h"

#define TAG "MagSceneEmulateConfig"

static const char* gpio[] = {
    [MagPinA7] = "2 (A7)",
    [MagPinA6] = "3 (A6)",
    [MagPinA4] = "4 (A4)",
    [MagPinB3] = "5 (B3)",
    [MagPinB2] = "6 (B2)",
    [MagPinC3] = "7 (C3)",
    [MagPinC1] = "15 (C1)",
    [MagPinC0] = "16 (C0)",
};
const uint8_t GPIO_COUNT = COUNT_OF(gpio);

static void mag_scene_settings_set_gpio(VariableItem* item, MagPin* pin_out) {
    MagPin pin = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, gpio[pin]);
    *pin_out = pin;
}

static void mag_scene_settings_set_gpio_input(VariableItem* item) {
    Mag* mag = variable_item_get_context(item);
    mag_scene_settings_set_gpio(item, &mag->state.pin_input);
};

static void mag_scene_settings_set_gpio_output(VariableItem* item) {
    Mag* mag = variable_item_get_context(item);
    mag_scene_settings_set_gpio(item, &mag->state.pin_output);
};

static void mag_scene_settings_set_gpio_enable(VariableItem* item) {
    Mag* mag = variable_item_get_context(item);
    mag_scene_settings_set_gpio(item, &mag->state.pin_enable);
};

static void mag_pin_variable_item_list_add(
    Mag* mag,
    const char* label,
    MagPin pin,
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
        mag, "Input pin:", mag->state.pin_input, mag_scene_settings_set_gpio_input);
    mag_pin_variable_item_list_add(
        mag, "Output pin:", mag->state.pin_output, mag_scene_settings_set_gpio_output);
    mag_pin_variable_item_list_add(
        mag, "Enable pin:", mag->state.pin_enable, mag_scene_settings_set_gpio_enable);

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

    variable_item_list_reset(mag->variable_item_list);

    if(!mag_state_gpio_is_valid(&mag->state)) {
        scene_manager_next_scene(mag->scene_manager, MagSceneSettingsInvalid);
    }

    mag_state_save(&mag->state);
}