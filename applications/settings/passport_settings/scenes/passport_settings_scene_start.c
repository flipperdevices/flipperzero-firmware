#include <applications.h>
#include <lib/toolbox/value_index.h>

#include "../passport_settings_app.h"
#include "passport_settings_scene.h"

#define SCENE_EVENT_SELECT_BACKGROUND 0
#define SCENE_EVENT_SELECT_IMAGE 1
#define SCENE_EVENT_SELECT_NAME 2
#define SCENE_EVENT_SELECT_MOOD 3
#define SCENE_EVENT_SELECT_LEVEL 4
#define SCENE_EVENT_SELECT_XP_TEXT 5
#define SCENE_EVENT_SELECT_XP_MODE 6
#define SCENE_EVENT_SELECT_MULTIPAGE 7

#define XP_MODE_COUNT 7

const char* const xp_mode_count_text[XP_MODE_COUNT] =
    {"Bar", "%", "Inv. %", "Retro 3", "Retro 5", "Bar %", "None"};

const uint32_t xp_mode_value[XP_MODE_COUNT] = {
    XP_MODE_BAR,
    XP_MODE_PERCENT,
    XP_MODE_INVERTED_PERCENT,
    XP_MODE_RETRO_3,
    XP_MODE_RETRO_5,
    XP_MODE_BAR_PERCENT,
    XP_MODE_NONE};

#define PASSPORT_ON_OFF_COUNT 2

const char* const passport_on_off_text[PASSPORT_ON_OFF_COUNT] = {
    "OFF",
    "ON",
};

const uint32_t background_value[PASSPORT_ON_OFF_COUNT] = {false, true};

const uint32_t image_value[PASSPORT_ON_OFF_COUNT] = {false, true};

const uint32_t name_value[PASSPORT_ON_OFF_COUNT] = {false, true};

const uint32_t mood_value[PASSPORT_ON_OFF_COUNT] = {false, true};

const uint32_t level_value[PASSPORT_ON_OFF_COUNT] = {false, true};

const uint32_t xp_text_value[PASSPORT_ON_OFF_COUNT] = {false, true};

const uint32_t multipage_value[PASSPORT_ON_OFF_COUNT] = {false, true};

static void passport_settings_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    PassportSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void passport_settings_scene_start_background_changed(VariableItem* item) {
    PassportSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, passport_on_off_text[index]);
    app->settings.background = background_value[index];
}

static void passport_settings_scene_start_image_changed(VariableItem* item) {
    PassportSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, passport_on_off_text[index]);
    app->settings.image = image_value[index];
}

static void passport_settings_scene_start_name_changed(VariableItem* item) {
    PassportSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, passport_on_off_text[index]);
    app->settings.name = name_value[index];
}

static void passport_settings_scene_start_mood_changed(VariableItem* item) {
    PassportSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, passport_on_off_text[index]);
    app->settings.mood = mood_value[index];
}

static void passport_settings_scene_start_level_changed(VariableItem* item) {
    PassportSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, passport_on_off_text[index]);
    app->settings.level = level_value[index];
}

static void passport_settings_scene_start_xp_text_changed(VariableItem* item) {
    PassportSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, passport_on_off_text[index]);
    app->settings.xp_text = xp_text_value[index];
}

static void passport_settings_scene_start_xp_mode_changed(VariableItem* item) {
    PassportSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, xp_mode_count_text[index]);
    app->settings.xp_mode = index;
}

static void passport_settings_scene_start_multipage_changed(VariableItem* item) {
    PassportSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, passport_on_off_text[index]);
    app->settings.multipage = multipage_value[index];
}

void passport_settings_scene_start_on_enter(void* context) {
    PassportSettingsApp* app = context;
    VariableItemList* variable_item_list = app->variable_item_list;

    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(
        variable_item_list,
        "Background",
        PASSPORT_ON_OFF_COUNT,
        passport_settings_scene_start_background_changed,
        app);

    value_index =
        value_index_uint32(app->settings.background, background_value, PASSPORT_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, passport_on_off_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Image",
        PASSPORT_ON_OFF_COUNT,
        passport_settings_scene_start_image_changed,
        app);

    value_index = value_index_uint32(app->settings.image, image_value, PASSPORT_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, passport_on_off_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Name",
        PASSPORT_ON_OFF_COUNT,
        passport_settings_scene_start_name_changed,
        app);

    value_index = value_index_uint32(app->settings.name, name_value, PASSPORT_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, passport_on_off_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Mood",
        PASSPORT_ON_OFF_COUNT,
        passport_settings_scene_start_mood_changed,
        app);

    value_index = value_index_uint32(app->settings.mood, mood_value, PASSPORT_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, passport_on_off_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Level",
        PASSPORT_ON_OFF_COUNT,
        passport_settings_scene_start_level_changed,
        app);

    value_index = value_index_uint32(app->settings.level, level_value, PASSPORT_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, passport_on_off_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "XP Text",
        PASSPORT_ON_OFF_COUNT,
        passport_settings_scene_start_xp_text_changed,
        app);

    value_index = value_index_uint32(app->settings.xp_text, xp_text_value, PASSPORT_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, passport_on_off_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "XP Mode",
        XP_MODE_COUNT,
        passport_settings_scene_start_xp_mode_changed,
        app);

    value_index = value_index_uint32(app->settings.xp_mode, xp_mode_value, XP_MODE_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, xp_mode_count_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Multiple Pages",
        PASSPORT_ON_OFF_COUNT,
        passport_settings_scene_start_multipage_changed,
        app);

    value_index =
        value_index_uint32(app->settings.multipage, multipage_value, PASSPORT_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, passport_on_off_text[value_index]);

    variable_item_list_set_enter_callback(
        variable_item_list, passport_settings_scene_start_var_list_enter_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, PassportSettingsAppViewVarItemList);
}

bool passport_settings_scene_start_on_event(void* context, SceneManagerEvent sme) {
    PassportSettingsApp* app = context;
    bool consumed = false;

    if(sme.type == SceneManagerEventTypeCustom) {
        switch(sme.event) {
        case SCENE_EVENT_SELECT_BACKGROUND:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_IMAGE:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_NAME:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_MOOD:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_LEVEL:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_XP_TEXT:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_XP_MODE:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_MULTIPAGE:
            consumed = true;
            break;
        }
    }
    return consumed;
}

void passport_settings_scene_start_on_exit(void* context) {
    PassportSettingsApp* app = context;
    variable_item_list_reset(app->variable_item_list);
    passport_settings_save(&app->settings);
}
