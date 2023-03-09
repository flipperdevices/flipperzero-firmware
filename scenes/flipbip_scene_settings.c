#include "../flipbip.h"
#include "../crypto/memzero.h"
#include <lib/toolbox/value_index.h>

const char* const haptic_text[2] = {
    "OFF",
    "ON",
};
const uint32_t haptic_value[2] = {
    FlipBipHapticOff,
    FlipBipHapticOn,
};

const char* const led_text[2] = {
    "OFF",
    "ON",
};
const uint32_t led_value[2] = {
    FlipBipLedOff,
    FlipBipLedOn,
};

const char* const bip39_strength_text[3] = {
    "12",
    "18",
    "24",
};
const uint32_t bip39_strength_value[3] = {
    FlipBipStrength128,
    FlipBipStrength192,
    FlipBipStrength256,
};

const char* const passphrase_text[2] = {
    "OFF",
    "ON",
};
const uint32_t passphrase_value[2] = {
    FlipBipPassphraseOff,
    FlipBipPassphraseOn,
};

static void flipbip_scene_settings_set_haptic(VariableItem* item) {
    FlipBip* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, haptic_text[index]);
    app->haptic = haptic_value[index];
}

static void flipbip_scene_settings_set_led(VariableItem* item) {
    FlipBip* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, led_text[index]);
    app->led = led_value[index];
}

static void flipbip_scene_settings_set_bip39_strength(VariableItem* item) {
    FlipBip* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, bip39_strength_text[index]);
    app->bip39_strength = bip39_strength_value[index];
}

static void flipbip_scene_settings_set_passphrase(VariableItem* item) {
    FlipBip* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, passphrase_text[index]);
    app->passphrase = passphrase_value[index];

    if(app->passphrase == FlipBipPassphraseOn) {
        app->input_state = FlipBipTextInputPassphrase;
        view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdTextInput);
    } else {
        memzero(app->passphrase_text, TEXT_BUFFER_SIZE);
    }
}

void flipbip_scene_settings_submenu_callback(void* context, uint32_t index) {
    FlipBip* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipbip_scene_settings_on_enter(void* context) {
    FlipBip* app = context;
    VariableItem* item;
    uint8_t value_index;

    // BIP39 strength
    item = variable_item_list_add(
        app->variable_item_list, "BIP39 Words:", 3, flipbip_scene_settings_set_bip39_strength, app);
    value_index = value_index_uint32(app->bip39_strength, bip39_strength_value, 3);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, bip39_strength_text[value_index]);

    // Passphrase
    item = variable_item_list_add(
        app->variable_item_list,
        "BIP39 Passphrase:",
        2,
        flipbip_scene_settings_set_passphrase,
        app);
    value_index = value_index_uint32(app->passphrase, passphrase_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, passphrase_text[value_index]);

    // Vibro on/off
    item = variable_item_list_add(
        app->variable_item_list, "Vibro/Haptic:", 2, flipbip_scene_settings_set_haptic, app);
    value_index = value_index_uint32(app->haptic, haptic_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, haptic_text[value_index]);

    // LED Effects on/off
    item = variable_item_list_add(
        app->variable_item_list, "LED FX:", 2, flipbip_scene_settings_set_led, app);
    value_index = value_index_uint32(app->led, led_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, led_text[value_index]);

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdSettings);
}

bool flipbip_scene_settings_on_event(void* context, SceneManagerEvent event) {
    FlipBip* app = context;
    UNUSED(app);
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
    }
    return consumed;
}

void flipbip_scene_settings_on_exit(void* context) {
    FlipBip* app = context;
    variable_item_list_set_selected_item(app->variable_item_list, 0);
    variable_item_list_reset(app->variable_item_list);
}