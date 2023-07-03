#include "../camera_suite.h"
#include <lib/toolbox/value_index.h>

const char* const haptic_text[2] = {
    "OFF",
    "ON",
};

const uint32_t haptic_value[2] = {
    CameraSuiteHapticOff,
    CameraSuiteHapticOn,
};

const char* const speaker_text[2] = {
    "OFF",
    "ON",
};

const uint32_t speaker_value[2] = {
    CameraSuiteSpeakerOff,
    CameraSuiteSpeakerOn,
};

const char* const led_text[2] = {
    "OFF",
    "ON",
};

const uint32_t led_value[2] = {
    CameraSuiteLedOff,
    CameraSuiteLedOn,
};

static void camera_suite_scene_settings_set_haptic(VariableItem* item) {
    CameraSuite* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, haptic_text[index]);
    app->haptic = haptic_value[index];
}

static void camera_suite_scene_settings_set_speaker(VariableItem* item) {
    CameraSuite* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, speaker_text[index]);
    app->speaker = speaker_value[index];
}

static void camera_suite_scene_settings_set_led(VariableItem* item) {
    CameraSuite* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, led_text[index]);
    app->led = led_value[index];
}

void camera_suite_scene_settings_submenu_callback(void* context, uint32_t index) {
    CameraSuite* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void camera_suite_scene_settings_on_enter(void* context) {
    CameraSuite* app = context;
    VariableItem* item;
    uint8_t value_index;

    // Haptic FX ON/OFF
    item = variable_item_list_add(
        app->variable_item_list, "Haptic FX:", 2, camera_suite_scene_settings_set_haptic, app);
    value_index = value_index_uint32(app->haptic, haptic_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, haptic_text[value_index]);

    // Sound FX ON/OFF
    item = variable_item_list_add(
        app->variable_item_list, "Sound FX:", 2, camera_suite_scene_settings_set_speaker, app);
    value_index = value_index_uint32(app->speaker, speaker_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, speaker_text[value_index]);

    // LED FX ON/OFF
    item = variable_item_list_add(
        app->variable_item_list, "LED FX:", 2, camera_suite_scene_settings_set_led, app);
    value_index = value_index_uint32(app->led, led_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, led_text[value_index]);

    view_dispatcher_switch_to_view(app->view_dispatcher, CameraSuiteViewIdSettings);
}

bool camera_suite_scene_settings_on_event(void* context, SceneManagerEvent event) {
    CameraSuite* app = context;
    UNUSED(app);
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
    }
    return consumed;
}

void camera_suite_scene_settings_on_exit(void* context) {
    CameraSuite* app = context;
    variable_item_list_set_selected_item(app->variable_item_list, 0);
    variable_item_list_reset(app->variable_item_list);
}