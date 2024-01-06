#include "../meal_pager_i.h"
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
    Meal_PagerHapticOff,
    Meal_PagerHapticOn,
};

const char* const pager_type_text[4] = {
    "T119",
    "TD157",
    "TD165",
    "TD174",
};

const uint32_t pager_type_value[4] = {
    Meal_PagerPagerTypeT119,
    Meal_PagerPagerTypeTD157,
    Meal_PagerPagerTypeTD165,
    Meal_PagerPagerTypeTD174,
};

const char* const speaker_text[2] = {
    "OFF",
    "ON",
};
const uint32_t speaker_value[2] = {
    Meal_PagerSpeakerOff,
    Meal_PagerSpeakerOn,
};

const char* const led_text[2] = {
    "OFF",
    "ON",
};
const uint32_t led_value[2] = {
    Meal_PagerLedOff,
    Meal_PagerLedOn,
};

const char* const settings_text[2] = {
    "OFF",
    "ON",
};
const uint32_t settings_value[2] = {
    Meal_PagerSettingsOff,
    Meal_PagerSettingsOn,
};

static void meal_pager_scene_settings_set_pager_type(VariableItem* item) {
    Meal_Pager* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, pager_type_text[index]);
    app->pager_type = pager_type_value[index];
}

static void meal_pager_scene_settings_set_first_station(VariableItem* item) {
    Meal_Pager* app = variable_item_get_context(item);
    uint32_t index = variable_item_get_current_value_index(item);

    snprintf(app->first_station_char, 20, "%lu", index);
    variable_item_set_current_value_text(item, app->first_station_char);
    app->first_station = index;
}

static void meal_pager_scene_settings_set_last_station(VariableItem* item) {
    Meal_Pager* app = variable_item_get_context(item);
    uint32_t index = variable_item_get_current_value_index(item);

    snprintf(app->last_station_char, 20, "%lu", index);
    variable_item_set_current_value_text(item, app->last_station_char);
    app->last_station = index;
}

static void meal_pager_scene_settings_set_first_pager(VariableItem* item) {
    Meal_Pager* app = variable_item_get_context(item);
    uint32_t index = variable_item_get_current_value_index(item);

    snprintf(app->first_pager_char, 20, "%lu", index);
    variable_item_set_current_value_text(item, app->first_pager_char);
    app->first_pager = index;
}

static void meal_pager_scene_settings_set_last_pager(VariableItem* item) {
    Meal_Pager* app = variable_item_get_context(item);
    uint32_t index = variable_item_get_current_value_index(item);

    snprintf(app->last_pager_char, 20, "%lu", index);
    variable_item_set_current_value_text(item, app->last_pager_char);
    app->last_pager = index;
}

static void meal_pager_scene_settings_set_repeats(VariableItem* item) {
    Meal_Pager* app = variable_item_get_context(item);
    uint32_t index = variable_item_get_current_value_index(item);

    snprintf(app->repeats_char, 20, "%lu", index);
    variable_item_set_current_value_text(item, app->repeats_char);
    app->repeats = index;
}

static void meal_pager_scene_settings_set_haptic(VariableItem* item) {
    Meal_Pager* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, haptic_text[index]);
    app->haptic = haptic_value[index];
}

static void meal_pager_scene_settings_set_speaker(VariableItem* item) {
    Meal_Pager* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, speaker_text[index]);
    app->speaker = speaker_value[index];
}

static void meal_pager_scene_settings_set_led(VariableItem* item) {
    Meal_Pager* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, led_text[index]);
    app->led = led_value[index];
}

static void meal_pager_scene_settings_set_save_settings(VariableItem* item) {
    Meal_Pager* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, settings_text[index]);
    app->save_settings = settings_value[index];
}

void meal_pager_scene_settings_submenu_callback(void* context, uint32_t index) {
    Meal_Pager* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void meal_pager_scene_settings_on_enter(void* context) {
    Meal_Pager* app = context;
    VariableItem* item;
    uint8_t value_index;

    // Pager Type
    item = variable_item_list_add(
        app->variable_item_list,
        "Pager Type:",
        4,
        meal_pager_scene_settings_set_pager_type,
        app);
    value_index = value_index_uint32(app->pager_type, pager_type_value, 4);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, pager_type_text[value_index]);

    // First Station
    item = variable_item_list_add(
        app->variable_item_list,
        "First Station",
        255,
        meal_pager_scene_settings_set_first_station,
        app);
    variable_item_set_current_value_index(item, app->first_station);
    snprintf(app->first_pager_char, 20, "%lu", app->first_station);
    variable_item_set_current_value_text(item, app->first_station_char);

    // Last Station
    item = variable_item_list_add(
        app->variable_item_list,
        "Last Station",
        255,
        meal_pager_scene_settings_set_last_station,
        app);
    variable_item_set_current_value_index(item, app->last_station);
    snprintf(app->last_station_char, 20, "%lu", app->last_station);
    variable_item_set_current_value_text(item, app->last_station_char);

    // First Pager
    item = variable_item_list_add(
        app->variable_item_list, "First Pager", 99, meal_pager_scene_settings_set_first_pager, app);
    variable_item_set_current_value_index(item, app->first_pager);
    snprintf(app->first_pager_char, 20, "%lu", app->first_pager);
    variable_item_set_current_value_text(item, app->first_pager_char);

    // Last Pager
    item = variable_item_list_add(
        app->variable_item_list, "Last Pager", 99, meal_pager_scene_settings_set_last_pager, app);
    variable_item_set_current_value_index(item, app->last_pager);
    snprintf(app->last_pager_char, 20, "%lu", app->last_pager);
    variable_item_set_current_value_text(item, app->last_pager_char);

    // Repeat Attacks
    item = variable_item_list_add(
        app->variable_item_list, "Signal Repeat", 11, meal_pager_scene_settings_set_repeats, app);
    variable_item_set_current_value_index(item, app->repeats);
    snprintf(app->repeats_char, 20, "%lu", app->repeats);
    variable_item_set_current_value_text(item, app->repeats_char);

    // Vibro on/off
    item = variable_item_list_add(
        app->variable_item_list, "Vibro/Haptic:", 2, meal_pager_scene_settings_set_haptic, app);
    value_index = value_index_uint32(app->haptic, haptic_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, haptic_text[value_index]);

    // Sound on/off
    item = variable_item_list_add(
        app->variable_item_list, "Sound:", 2, meal_pager_scene_settings_set_speaker, app);
    value_index = value_index_uint32(app->speaker, speaker_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, speaker_text[value_index]);

    // LED Effects on/off
    item = variable_item_list_add(
        app->variable_item_list, "LED FX:", 2, meal_pager_scene_settings_set_led, app);
    value_index = value_index_uint32(app->led, led_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, led_text[value_index]);

    // Save Settings to File
    item = variable_item_list_add(
        app->variable_item_list,
        "Save Settings",
        2,
        meal_pager_scene_settings_set_save_settings,
        app);
    value_index = value_index_uint32(app->save_settings, settings_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, settings_text[value_index]);

    view_dispatcher_switch_to_view(app->view_dispatcher, Meal_PagerViewIdSettings);
}

bool meal_pager_scene_settings_on_event(void* context, SceneManagerEvent event) {
    Meal_Pager* app = context;
    UNUSED(app);
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
    }
    return consumed;
}

void meal_pager_scene_settings_on_exit(void* context) {
    Meal_Pager* app = context;
    variable_item_list_set_selected_item(app->variable_item_list, 0);
    variable_item_list_reset(app->variable_item_list);
}