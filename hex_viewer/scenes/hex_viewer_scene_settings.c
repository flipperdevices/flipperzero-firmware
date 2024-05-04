#include "../hex_viewer.h"
#include <lib/toolbox/value_index.h>

enum SettingsIndex {
    SettingsIndexHaptic = 10,
    SettingsIndexValue1,
    SettingsIndexValue2,
};

const char* const haptic_text[2] = {
    "OFF",
    "ON",
};
const uint32_t haptic_value[2] = {
    HexViewerHapticOff,
    HexViewerHapticOn,
};

const char* const speaker_text[2] = {
    "OFF",
    "ON",
};
const uint32_t speaker_value[2] = {
    HexViewerSpeakerOff,
    HexViewerSpeakerOn,
};

const char* const led_text[2] = {
    "OFF",
    "ON",
};
const uint32_t led_value[2] = {
    HexViewerLedOff,
    HexViewerLedOn,
};

const char* const settings_text[2] = {
    "OFF",
    "ON",
};
const uint32_t settings_value[2] = {
    HexViewerSettingsOff,
    HexViewerSettingsOn,
};

static void hex_viewer_scene_settings_set_haptic(VariableItem* item) {
    HexViewer* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, haptic_text[index]);
    app->haptic = haptic_value[index];
}

static void hex_viewer_scene_settings_set_speaker(VariableItem* item) {
    HexViewer* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, speaker_text[index]);
    app->speaker = speaker_value[index];
}

static void hex_viewer_scene_settings_set_led(VariableItem* item) {
    HexViewer* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, led_text[index]);
    app->led = led_value[index];
}

static void hex_viewer_scene_settings_set_save_settings(VariableItem* item) {
    HexViewer* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, settings_text[index]);
    app->save_settings = settings_value[index];
}

void hex_viewer_scene_settings_submenu_callback(void* context, uint32_t index) {
    HexViewer* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void hex_viewer_scene_settings_on_enter(void* context) {
    HexViewer* app = context;
    VariableItem* item;
    uint8_t value_index;

    // Vibro on/off
    item = variable_item_list_add(
        app->variable_item_list, "Vibro/Haptic:", 2, hex_viewer_scene_settings_set_haptic, app);
    value_index = value_index_uint32(app->haptic, haptic_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, haptic_text[value_index]);

    // Sound on/off
    item = variable_item_list_add(
        app->variable_item_list, "Sound:", 2, hex_viewer_scene_settings_set_speaker, app);
    value_index = value_index_uint32(app->speaker, speaker_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, speaker_text[value_index]);

    // LED Effects on/off
    item = variable_item_list_add(
        app->variable_item_list, "LED FX:", 2, hex_viewer_scene_settings_set_led, app);
    value_index = value_index_uint32(app->led, led_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, led_text[value_index]);

    // Save Settings to File
    item = variable_item_list_add(
        app->variable_item_list,
        "Save Settings",
        2,
        hex_viewer_scene_settings_set_save_settings,
        app);
    value_index = value_index_uint32(app->save_settings, settings_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, settings_text[value_index]);

    view_dispatcher_switch_to_view(app->view_dispatcher, HexViewerViewIdSettings);
}

bool hex_viewer_scene_settings_on_event(void* context, SceneManagerEvent event) {
    HexViewer* app = context;
    UNUSED(app);
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
    }
    return consumed;
}

void hex_viewer_scene_settings_on_exit(void* context) {
    HexViewer* app = context;
    variable_item_list_set_selected_item(app->variable_item_list, 0);
    variable_item_list_reset(app->variable_item_list);
}