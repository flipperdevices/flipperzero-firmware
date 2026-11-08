#include "../power_settings_app.h"

#define LOW_BATTERY_THRESHOLD_COUNT (9)
static const char* const low_battery_threshold_text[LOW_BATTERY_THRESHOLD_COUNT] = {
    "OFF",
    "5%",
    "10%",
    "15%",
    "20%",
    "25%",
    "30%",
    "40%",
    "50%",
};
static const uint8_t low_battery_threshold_value[LOW_BATTERY_THRESHOLD_COUNT] =
    {0, 5, 10, 15, 20, 25, 30, 40, 50};

#define ALERT_SCREEN_COUNT (2)
static const char* const alert_screen_text[ALERT_SCREEN_COUNT] = {
    "OFF",
    "ON",
};

#define REPEAT_STEP_COUNT (4)
static const char* const repeat_step_text[REPEAT_STEP_COUNT] = {
    "OFF",
    "Each 1%",
    "Each 5%",
    "Each 10%",
};
static const uint8_t repeat_step_value[REPEAT_STEP_COUNT] = {0, 1, 5, 10};

static void power_settings_scene_battery_alert_threshold_changed(VariableItem* item) {
    PowerSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, low_battery_threshold_text[index]);
    app->settings.low_battery_threshold = low_battery_threshold_value[index];
}

static void power_settings_scene_battery_alert_screen_changed(VariableItem* item) {
    PowerSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, alert_screen_text[index]);
    app->settings.low_battery_show_popup = index;
}

static void power_settings_scene_battery_alert_repeat_changed(VariableItem* item) {
    PowerSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, repeat_step_text[index]);
    app->settings.low_battery_repeat_step = repeat_step_value[index];
}

static uint8_t power_settings_scene_battery_alert_value_index(
    uint8_t value,
    const uint8_t values[],
    uint8_t values_count) {
    // Pick the closest option not above the stored value
    uint8_t index = 0;
    for(uint8_t i = 0; i < values_count; i++) {
        if(values[i] <= value) {
            index = i;
        }
    }
    return index;
}

void power_settings_scene_battery_alert_on_enter(void* context) {
    PowerSettingsApp* app = context;
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(
        var_item_list,
        "Low Battery Alert",
        LOW_BATTERY_THRESHOLD_COUNT,
        power_settings_scene_battery_alert_threshold_changed,
        app);
    value_index = power_settings_scene_battery_alert_value_index(
        app->settings.low_battery_threshold,
        low_battery_threshold_value,
        LOW_BATTERY_THRESHOLD_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, low_battery_threshold_text[value_index]);

    item = variable_item_list_add(
        var_item_list,
        "Repeat",
        REPEAT_STEP_COUNT,
        power_settings_scene_battery_alert_repeat_changed,
        app);
    value_index = power_settings_scene_battery_alert_value_index(
        app->settings.low_battery_repeat_step, repeat_step_value, REPEAT_STEP_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, repeat_step_text[value_index]);

    item = variable_item_list_add(
        var_item_list,
        "Alert Screen",
        ALERT_SCREEN_COUNT,
        power_settings_scene_battery_alert_screen_changed,
        app);
    value_index = app->settings.low_battery_show_popup ? 1 : 0;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, alert_screen_text[value_index]);

    view_dispatcher_switch_to_view(app->view_dispatcher, PowerSettingsAppViewVarItemList);
}

bool power_settings_scene_battery_alert_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void power_settings_scene_battery_alert_on_exit(void* context) {
    PowerSettingsApp* app = context;
    power_set_settings(app->power, &app->settings);
    variable_item_list_reset(app->var_item_list);
}
