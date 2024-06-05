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

const char* rotors_model_text[8] = {
    "M3-I",
    "M3-II",
    "M3-III",
    "M3-IV",
    "M3-V",
    "M4-VI",
    "M4-VII",
    "M4-VIII",
};
const uint32_t rotors_model_value[8] = {
    FlipEnigmaRotorM3I,
    FlipEnigmaRotorM3II,
    FlipEnigmaRotorM3III,
    FlipEnigmaRotorM3IV,
    FlipEnigmaRotorM3V,
    FlipEnigmaRotorM4VI,
    FlipEnigmaRotorM4VII,
    FlipEnigmaRotorM4VIII,
};

const char* reflector_model_text[3] = {
    "M3-A",
    "M3-B",
    "M3-C",
};
const uint32_t reflector_model_value[3] = {
    FlipEnigmaReflectorM3A,
    FlipEnigmaReflectorM3B,
    FlipEnigmaReflectorM3C,
};

const char* rotor_positions_text[26] = {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
};
const char* rotor_ring_settings_text[26] = {
    "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",  "10", "11", "12", "13",
    "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26",
};
const uint32_t rotor_positions_value[26] = {
    FlipEnigma01A, FlipEnigma02B, FlipEnigma03C, FlipEnigma04D, FlipEnigma05E, FlipEnigma06F,
    FlipEnigma07G, FlipEnigma08H, FlipEnigma09I, FlipEnigma10J, FlipEnigma11K, FlipEnigma12L,
    FlipEnigma13M, FlipEnigma14N, FlipEnigma15O, FlipEnigma16P, FlipEnigma17Q, FlipEnigma18R,
    FlipEnigma19S, FlipEnigma20T, FlipEnigma21U, FlipEnigma22V, FlipEnigma23W, FlipEnigma24X,
    FlipEnigma25Y, FlipEnigma26Z,
};

uint32_t find_index(const char* value, const char* values[], size_t values_len) {
    for(size_t i = 0; i < values_len; ++i) {
        if(strcmp(values[i], value) == 0) {
            return i;
        }
    }
    return 99; // Return 99 if not found
}

static void flipenigma_scene_settings_set_haptic(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, haptic_text[index]);
    app->haptic = haptic_value[index];
}

static void flipenigma_scene_settings_set_reflector(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, reflector_model_text[index]);
    app->reflector_model = reflector_model_text[index];
}

static void flipenigma_scene_settings_set_rotor_1(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rotors_model_text[index]);
    app->rotors_model[0] = rotors_model_text[index];
}

static void flipenigma_scene_settings_set_rotor_1_position(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rotor_positions_text[index]);
    app->rotor_positions[0] = rotor_positions_value[index];
}

static void flipenigma_scene_settings_set_rotor_1_ring_setting(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rotor_ring_settings_text[index]);
    app->rotor_ring_settings[0] = rotor_positions_value[index];
}

static void flipenigma_scene_settings_set_rotor_2(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rotors_model_text[index]);
    app->rotors_model[1] = rotors_model_text[index];
}

static void flipenigma_scene_settings_set_rotor_2_position(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rotor_positions_text[index]);
    app->rotor_positions[1] = rotor_positions_value[index];
}

static void flipenigma_scene_settings_set_rotor_2_ring_setting(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rotor_ring_settings_text[index]);
    app->rotor_ring_settings[1] = rotor_positions_value[index];
}

static void flipenigma_scene_settings_set_rotor_3_position(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rotor_positions_text[index]);
    app->rotor_positions[2] = rotor_positions_value[index];
}

static void flipenigma_scene_settings_set_rotor_3_ring_setting(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rotor_ring_settings_text[index]);
    app->rotor_ring_settings[2] = rotor_positions_value[index];
}

static void flipenigma_scene_settings_set_rotor_3(VariableItem* item) {
    FlipEnigma* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rotors_model_text[index]);
    app->rotors_model[2] = rotors_model_text[index];
}

void flipenigma_scene_settings_submenu_callback(void* context, uint32_t index) {
    FlipEnigma* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipenigma_scene_settings_on_enter(void* context) {
    FlipEnigma* app = context;
    VariableItem* item;
    uint8_t value_index;

    // Reflector model
    item = variable_item_list_add(
        app->variable_item_list, "Reflector:", 3, flipenigma_scene_settings_set_reflector, app);
    value_index = value_index_uint32(
        find_index(app->reflector_model, reflector_model_text, 3), reflector_model_value, 3);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, reflector_model_text[value_index]);

    // Rotor 1 model
    item = variable_item_list_add(
        app->variable_item_list, "Rotor 1:", 8, flipenigma_scene_settings_set_rotor_1, app);
    value_index = value_index_uint32(
        find_index(app->rotors_model[0], rotors_model_text, 8), rotors_model_value, 8);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rotors_model_text[value_index]);

    item = variable_item_list_add(
        app->variable_item_list,
        "-> Position:",
        26,
        flipenigma_scene_settings_set_rotor_1_position,
        app);
    value_index = value_index_uint32(app->rotor_positions[0], rotor_positions_value, 26);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rotor_positions_text[value_index]);

    item = variable_item_list_add(
        app->variable_item_list,
        "-> Ring Setting:",
        26,
        flipenigma_scene_settings_set_rotor_1_ring_setting,
        app);
    value_index = value_index_uint32(app->rotor_ring_settings[0], rotor_positions_value, 26);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rotor_ring_settings_text[value_index]);

    // Rotor 2 model
    item = variable_item_list_add(
        app->variable_item_list, "Rotor 2:", 8, flipenigma_scene_settings_set_rotor_2, app);
    value_index = value_index_uint32(
        find_index(app->rotors_model[1], rotors_model_text, 8), rotors_model_value, 8);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rotors_model_text[value_index]);

    item = variable_item_list_add(
        app->variable_item_list,
        "-> Position:",
        26,
        flipenigma_scene_settings_set_rotor_2_position,
        app);
    value_index = value_index_uint32(app->rotor_positions[1], rotor_positions_value, 26);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rotor_positions_text[value_index]);

    item = variable_item_list_add(
        app->variable_item_list,
        "-> Ring Setting:",
        26,
        flipenigma_scene_settings_set_rotor_2_ring_setting,
        app);
    value_index = value_index_uint32(app->rotor_ring_settings[1], rotor_positions_value, 26);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rotor_ring_settings_text[value_index]);

    // Rotor 3 model
    item = variable_item_list_add(
        app->variable_item_list, "Rotor 3:", 8, flipenigma_scene_settings_set_rotor_3, app);
    value_index = value_index_uint32(
        find_index(app->rotors_model[2], rotors_model_text, 8), rotors_model_value, 8);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rotors_model_text[value_index]);

    item = variable_item_list_add(
        app->variable_item_list,
        "-> Position:",
        26,
        flipenigma_scene_settings_set_rotor_3_position,
        app);
    value_index = value_index_uint32(app->rotor_positions[2], rotor_positions_value, 26);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rotor_positions_text[value_index]);

    item = variable_item_list_add(
        app->variable_item_list,
        "-> Ring Setting:",
        26,
        flipenigma_scene_settings_set_rotor_3_ring_setting,
        app);
    value_index = value_index_uint32(app->rotor_ring_settings[2], rotor_positions_value, 26);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rotor_ring_settings_text[value_index]);

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