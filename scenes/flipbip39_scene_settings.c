#include "../flipbip39.h"
#include <lib/toolbox/value_index.h>

enum SettingsIndex {
    SettingsIndexBip39Strength = 10,
    SettingsIndexHaptic,
    SettingsIndexValue1,
    SettingsIndexValue2,
};

const char* const haptic_text[2] = {
    "OFF",
    "ON",
};
const uint32_t haptic_value[2] = {
    FlipBip39HapticOff,
    FlipBip39HapticOn,
};

const char* const speaker_text[2] = {
    "OFF",
    "ON",
};
const uint32_t speaker_value[2] = {
    FlipBip39SpeakerOff,
    FlipBip39SpeakerOn,
};

const char* const led_text[2] = {
    "OFF",
    "ON",
};
const uint32_t led_value[2] = {
    FlipBip39LedOff,
    FlipBip39LedOn,
};

const char* const bip39_strength_text[2] = {
    "24",
    "12",
};
const uint32_t bip39_strength_value[2] = {
    FlipBip39Strength256,
    FlipBip39Strength128,
};

static void flipbip39_scene_settings_set_haptic(VariableItem* item) {
    FlipBip39* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, haptic_text[index]);
    app->haptic = haptic_value[index];
}

static void flipbip39_scene_settings_set_speaker(VariableItem* item) {
    FlipBip39* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, speaker_text[index]);
    app->speaker = speaker_value[index];
}

static void flipbip39_scene_settings_set_led(VariableItem* item) {
    FlipBip39* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, led_text[index]);
    app->led = led_value[index];
}

static void flipbip39_scene_settings_set_bip39_strength(VariableItem* item) {
    FlipBip39* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, bip39_strength_text[index]);
    app->bip39_strength = bip39_strength_value[index];
}

void flipbip39_scene_settings_submenu_callback(void* context, uint32_t index) {
    FlipBip39* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipbip39_scene_settings_on_enter(void* context) {
    FlipBip39* app = context;
    VariableItem* item;
    uint8_t value_index;

    // BIP39 strength
    item = variable_item_list_add(
        app->variable_item_list,
        "BIP39 Words:",
        2,
        flipbip39_scene_settings_set_bip39_strength,
        app);
    value_index = value_index_uint32(app->bip39_strength, bip39_strength_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, bip39_strength_text[value_index]);

    // Vibro on/off
    item = variable_item_list_add(
        app->variable_item_list,
        "Vibro/Haptic:",
        2,
        flipbip39_scene_settings_set_haptic,
        app);
    value_index = value_index_uint32(app->haptic, haptic_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, haptic_text[value_index]);

    // Sound on/off
    item = variable_item_list_add(
        app->variable_item_list,
        "Sound:",
        2,
        flipbip39_scene_settings_set_speaker,
        app);
    value_index = value_index_uint32(app->speaker, speaker_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, speaker_text[value_index]);

    // LED Effects on/off
    item = variable_item_list_add(
        app->variable_item_list,
        "LED FX:",
        2,
        flipbip39_scene_settings_set_led,
        app);
    value_index = value_index_uint32(app->led, led_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, led_text[value_index]);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, FlipBip39ViewIdSettings);
}

bool flipbip39_scene_settings_on_event(void* context, SceneManagerEvent event) {
    FlipBip39* app = context;
    UNUSED(app);
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        
    }
    return consumed;
}

void flipbip39_scene_settings_on_exit(void* context) {
    FlipBip39* app = context;
    variable_item_list_set_selected_item(app->variable_item_list, 0);
    variable_item_list_reset(app->variable_item_list);
}