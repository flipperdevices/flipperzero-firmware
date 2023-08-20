#include <applications.h>
#include <lib/toolbox/value_index.h>

#include "../desktop_settings_app.h"
#include "desktop_settings_scene.h"
#include "desktop_settings_scene_i.h"
#include <power/power_service/power.h>

typedef enum {
    DesktopSettingsPinSetup = 0,
    DesktopSettingsAutoLockDelay,
    DesktopSettingsAutoLockPin,
    DesktopSettingsDumbMode,
    DesktopSettingsFavoriteDownLong,
    DesktopSettingsFavoriteLeftShort,
    DesktopSettingsFavoriteLeftLong,
    DesktopSettingsFavoriteRightShort,
    DesktopSettingsFavoriteRightLong,
    DesktopSettingsFavoriteUpLong,
    DesktopSettingsDummyLeft,
    DesktopSettingsDummyLeftLong,
    DesktopSettingsDummyRight,
    DesktopSettingsDummyRightLong,
    DesktopSettingsDummyUpLong,
    DesktopSettingsDummyDown,
    DesktopSettingsDummyDownLong,
    DesktopSettingsDummyOk,
    DesktopSettingsDummyOkLong,
} DesktopSettingsEntry;

#define AUTO_LOCK_DELAY_COUNT 9
const char* const auto_lock_delay_text[AUTO_LOCK_DELAY_COUNT] = {
    "OFF",
    "10s",
    "15s",
    "30s",
    "60s",
    "90s",
    "2min",
    "5min",
    "10min",
};

const uint32_t auto_lock_delay_value[AUTO_LOCK_DELAY_COUNT] =
    {0, 10000, 15000, 30000, 60000, 90000, 120000, 300000, 600000};

#define DESKTOP_ON_OFF_COUNT 2

const char* const desktop_on_off_text[DESKTOP_ON_OFF_COUNT] = {
    "OFF",
    "ON",
};

const uint32_t dumbmode_value[DESKTOP_ON_OFF_COUNT] = {false, true};

static void desktop_settings_scene_start_var_list_enter_callback(void* context, uint32_t index) {
    DesktopSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void desktop_settings_scene_start_auto_lock_delay_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, auto_lock_delay_text[index]);
    app->settings.auto_lock_delay_ms = auto_lock_delay_value[index];
}

static void desktop_settings_scene_start_auto_lock_pin_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t value = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    app->settings.auto_lock_with_pin = value;
}

static void desktop_settings_scene_start_dumbmode_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, desktop_on_off_text[index]);
    app->settings.is_dumbmode = dumbmode_value[index];
}

void desktop_settings_scene_start_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    VariableItemList* variable_item_list = app->variable_item_list;

    VariableItem* item;
    uint8_t value_index;

    variable_item_list_add(variable_item_list, "PIN Setup", 1, NULL, NULL);

    item = variable_item_list_add(
        variable_item_list,
        "Auto Lock Time",
        AUTO_LOCK_DELAY_COUNT,
        desktop_settings_scene_start_auto_lock_delay_changed,
        app);

    value_index = value_index_uint32(
        app->settings.auto_lock_delay_ms, auto_lock_delay_value, AUTO_LOCK_DELAY_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, auto_lock_delay_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Auto Lock Pin",
        2,
        desktop_settings_scene_start_auto_lock_pin_changed,
        app);

    variable_item_set_current_value_index(item, app->settings.auto_lock_with_pin);
    variable_item_set_current_value_text(item, app->settings.auto_lock_with_pin ? "ON" : "OFF");

    item = variable_item_list_add(
        variable_item_list,
        "Games Only",
        DESKTOP_ON_OFF_COUNT,
        desktop_settings_scene_start_dumbmode_changed,
        app);

    value_index =
        value_index_uint32(app->settings.is_dumbmode, dumbmode_value, DESKTOP_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, desktop_on_off_text[value_index]);

    variable_item_list_add(variable_item_list, "Favorite - Down Long", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "Favorite - Left Short", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "Favorite - Left Long", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "Favorite - Right Short", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "Favorite - Right Long", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "Favorite - Up Long", 1, NULL, NULL);

    variable_item_list_add(variable_item_list, "DummyMode - Left", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "DummyMode - Left Long", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "DummyMode - Right", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "DummyMode - Right Long", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "DummyMode - Up Long", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "DummyMode - Down", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "DummyMode - Down Long", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "DummyMode - Ok", 1, NULL, NULL);
    variable_item_list_add(variable_item_list, "DummyMode - Ok Long", 1, NULL, NULL);

    variable_item_list_set_enter_callback(
        variable_item_list, desktop_settings_scene_start_var_list_enter_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewVarItemList);
}

bool desktop_settings_scene_start_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DesktopSettingsPinSetup:
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppScenePinMenu);
            break;
        case DesktopSettingsAutoLockDelay:
            break;
        case DesktopSettingsAutoLockPin:
            break;
        case DesktopSettingsDumbMode:
            break;
        case DesktopSettingsFavoriteDownLong:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_FAVORITE_APP | FavoriteAppDownLong);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsFavoriteLeftShort:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_FAVORITE_APP | FavoriteAppLeftShort);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsFavoriteLeftLong:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_FAVORITE_APP | FavoriteAppLeftLong);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsFavoriteRightShort:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_FAVORITE_APP | FavoriteAppRightShort);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsFavoriteRightLong:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_FAVORITE_APP | FavoriteAppRightLong);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsFavoriteUpLong:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_FAVORITE_APP | FavoriteAppUpLong);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;

        case DesktopSettingsDummyLeft:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_DUMMY_APP | DummyAppLeft);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsDummyLeftLong:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_DUMMY_APP | DummyAppLeftLong);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsDummyRight:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_DUMMY_APP | DummyAppRight);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsDummyRightLong:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_DUMMY_APP | DummyAppRightLong);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsDummyUpLong:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_DUMMY_APP | DummyAppUpLong);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsDummyDown:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_DUMMY_APP | DummyAppDown);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsDummyDownLong:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_DUMMY_APP | DummyAppDownLong);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsDummyOk:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_DUMMY_APP | DummyAppOk);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        case DesktopSettingsDummyOkLong:
            scene_manager_set_scene_state(
                app->scene_manager,
                DesktopSettingsAppSceneFavorite,
                SCENE_STATE_SET_DUMMY_APP | DummyAppOkLong);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            break;
        default:
            break;
        }
        consumed = true;
    }
    return consumed;
}

void desktop_settings_scene_start_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    variable_item_list_reset(app->variable_item_list);
    DESKTOP_SETTINGS_SAVE(&app->settings);
}
