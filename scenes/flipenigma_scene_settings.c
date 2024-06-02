#include "../flipenigma.h"
#include <lib/toolbox/value_index.h>

#define TEXT_LABEL_ON "ON"
#define TEXT_LABEL_OFF "OFF"

const char* const haptic_text[2] = {
    TEXT_LABEL_OFF,
    TEXT_LABEL_ON,
};
const uint32_t haptic_value[2] = {
    FlipEnigmaHapticOff,
    FlipEnigmaHapticOn,
};

const char* const player_mode_text[4] = {
    "Human",
    "CPU 1",
    "CPU 2",
    "CPU 3",
};
const uint32_t player_mode_value[4] = {
    FlipEnigmaPlayerHuman,
    FlipEnigmaPlayerAI1,
    FlipEnigmaPlayerAI2,
    FlipEnigmaPlayerAI3,
};

static void flipenigma_scene_settings_set_haptic(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, haptic_text[index]);
    app->haptic = haptic_value[index];
}

static void flipenigma_scene_settings_set_white_mode(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, player_mode_text[index]);
    app->white_mode = player_mode_value[index];
}

static void flipenigma_scene_settings_set_black_mode(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, player_mode_text[index]);
    app->black_mode = player_mode_value[index];
}

void flipenigma_scene_settings_submenu_callback(void* context, uint32_t index) {
    FlipEnigma* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipenigma_scene_settings_on_enter(void* context) {
    FlipEnigma* app = context;
    VariableItem* item;
    uint8_t value_index;

    if(app->sound == 1) {
        // flipenigma_voice_which_side();
    }

    // White mode
    item = variable_item_list_add(
        app->variable_item_list, "White:", 4, flipenigma_scene_settings_set_white_mode, app);
    value_index = value_index_uint32(app->white_mode, player_mode_value, 4);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, player_mode_text[value_index]);

    // Black mode
    item = variable_item_list_add(
        app->variable_item_list, "Black:", 4, flipenigma_scene_settings_set_black_mode, app);
    value_index = value_index_uint32(app->black_mode, player_mode_value, 4);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, player_mode_text[value_index]);

    // Vibro on/off
    item = variable_item_list_add(
        app->variable_item_list, "Vibro/Haptic:", 2, flipenigma_scene_settings_set_haptic, app);
    value_index = value_index_uint32(app->haptic, haptic_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, haptic_text[value_index]);

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipEnigmaViewIdSettings);
}

bool flipenigma_scene_settings_on_event(void* context, SceneManagerEvent event) {
    FlipEnigma* app = context;
    UNUSED(app);
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
    }
    return consumed;
}

void flipenigma_scene_settings_on_exit(void* context) {
    FlipEnigma* app = context;
    variable_item_list_set_selected_item(app->variable_item_list, 0);
    variable_item_list_reset(app->variable_item_list);
}