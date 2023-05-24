#include <applications.h>
#include <lib/toolbox/value_index.h>

#include "../desktop_settings_app.h"
#include "desktop_settings_scene.h"
#include <power/power_service/power.h>

#define SCENE_EVENT_SELECT_FAVORITE_PRIMARY 0
#define SCENE_EVENT_SELECT_FAVORITE_SECONDARY 1
#define SCENE_EVENT_SELECT_FAVORITE_TERTIARY 2
#define SCENE_EVENT_SELECT_FAVORITE_QUATERNARY 3
#define SCENE_EVENT_SELECT_PIN_SETUP 4
#define SCENE_EVENT_SELECT_AUTO_LOCK_DELAY 5
#define SCENE_EVENT_SELECT_AUTO_LOCK_PIN 6
#define SCENE_EVENT_SELECT_ICON_STYLE 7
#define SCENE_EVENT_SELECT_BATTERY_DISPLAY 8
#define SCENE_EVENT_SELECT_LOCK_ICON 9
#define SCENE_EVENT_SELECT_BT_ICON 10
#define SCENE_EVENT_SELECT_RPC_ICON 11
#define SCENE_EVENT_SELECT_SDCARD_ICON 12
#define SCENE_EVENT_SELECT_STEALTH_ICON 13
#define SCENE_EVENT_SELECT_TOP_BAR 14
#define SCENE_EVENT_SELECT_DUMBMODE 15
#define SCENE_EVENT_SELECT_DUMBMODE_ICON 16

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

#define BATTERY_VIEW_COUNT 7

const char* const battery_view_count_text[BATTERY_VIEW_COUNT] =
    {"Bar", "%", "Inv. %", "Retro 3", "Retro 5", "Bar %", "None"};

const uint32_t displayBatteryPercentage_value[BATTERY_VIEW_COUNT] = {
    DISPLAY_BATTERY_BAR,
    DISPLAY_BATTERY_PERCENT,
    DISPLAY_BATTERY_INVERTED_PERCENT,
    DISPLAY_BATTERY_RETRO_3,
    DISPLAY_BATTERY_RETRO_5,
    DISPLAY_BATTERY_BAR_PERCENT,
    DISPLAY_BATTERY_NONE};

uint8_t origBattDisp_value = 0;

#define ICON_STYLE_COUNT 2

const char* const icon_style_count_text[ICON_STYLE_COUNT] = {"Stock", "Slim"};

const uint32_t icon_style_value[ICON_STYLE_COUNT] = {ICON_STYLE_STOCK, ICON_STYLE_SLIM};
uint8_t origIconStyle_value = 0;

#define DESKTOP_ON_OFF_COUNT 2

const char* const desktop_on_off_text[DESKTOP_ON_OFF_COUNT] = {
    "OFF",
    "ON",
};

const uint32_t lockicon_value[DESKTOP_ON_OFF_COUNT] = {false, true};

const uint32_t bticon_value[DESKTOP_ON_OFF_COUNT] = {false, true};

const uint32_t rpc_value[DESKTOP_ON_OFF_COUNT] = {false, true};

const uint32_t sdcard_value[DESKTOP_ON_OFF_COUNT] = {false, true};

const uint32_t stealth_value[DESKTOP_ON_OFF_COUNT] = {false, true};

const uint32_t topbar_value[DESKTOP_ON_OFF_COUNT] = {false, true};

const uint32_t dumbmode_value[DESKTOP_ON_OFF_COUNT] = {false, true};

const uint32_t dumbmode_icon_value[DESKTOP_ON_OFF_COUNT] = {false, true};

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

static void desktop_settings_scene_start_battery_view_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, battery_view_count_text[index]);
    app->settings.displayBatteryPercentage = index;
}

static void desktop_settings_scene_start_icon_style_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, icon_style_count_text[index]);
    app->settings.icon_style = icon_style_value[index];
}

static void desktop_settings_scene_start_lockicon_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, desktop_on_off_text[index]);
    app->settings.lock_icon = lockicon_value[index];
}

static void desktop_settings_scene_start_bticon_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, desktop_on_off_text[index]);
    app->settings.bt_icon = bticon_value[index];
}

static void desktop_settings_scene_start_rpc_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, desktop_on_off_text[index]);
    app->settings.rpc_icon = rpc_value[index];
}

static void desktop_settings_scene_start_sdcard_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, desktop_on_off_text[index]);
    app->settings.sdcard = sdcard_value[index];
}

static void desktop_settings_scene_start_stealth_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, desktop_on_off_text[index]);
    app->settings.stealth_icon = stealth_value[index];
}

static void desktop_settings_scene_start_topbar_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, desktop_on_off_text[index]);
    app->settings.top_bar = topbar_value[index];
}

static void desktop_settings_scene_start_dumbmode_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, desktop_on_off_text[index]);
    app->settings.is_dumbmode = dumbmode_value[index];
}

static void desktop_settings_scene_start_dumbmode_icon_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, desktop_on_off_text[index]);
    app->settings.dumbmode_icon = stealth_value[index];
}

static void desktop_settings_scene_start_auto_lock_pin_changed(VariableItem* item) {
    DesktopSettingsApp* app = variable_item_get_context(item);
    uint8_t value = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    app->settings.auto_lock_with_pin = value;
}

void desktop_settings_scene_start_on_enter(void* context) {
    DesktopSettingsApp* app = context;
    VariableItemList* variable_item_list = app->variable_item_list;
    origIconStyle_value = app->settings.icon_style;
    origBattDisp_value = app->settings.displayBatteryPercentage;

    VariableItem* item;
    uint8_t value_index;

    variable_item_list_add(variable_item_list, "Primary Favorite App", 1, NULL, NULL);

    variable_item_list_add(variable_item_list, "Secondary Favorite App", 1, NULL, NULL);

    variable_item_list_add(variable_item_list, "Tertiary Favorite App", 1, NULL, NULL);

    variable_item_list_add(variable_item_list, "Quaternary Favorite App", 1, NULL, NULL);

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
        "Icon Style",
        ICON_STYLE_COUNT,
        desktop_settings_scene_start_icon_style_changed,
        app);

    value_index = value_index_uint32(app->settings.icon_style, icon_style_value, ICON_STYLE_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, icon_style_count_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Battery View",
        BATTERY_VIEW_COUNT,
        desktop_settings_scene_start_battery_view_changed,
        app);

    value_index = value_index_uint32(
        app->settings.displayBatteryPercentage,
        displayBatteryPercentage_value,
        BATTERY_VIEW_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, battery_view_count_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Lock Icon",
        DESKTOP_ON_OFF_COUNT,
        desktop_settings_scene_start_lockicon_changed,
        app);

    value_index =
        value_index_uint32(app->settings.lock_icon, lockicon_value, DESKTOP_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, desktop_on_off_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "BT Icon",
        DESKTOP_ON_OFF_COUNT,
        desktop_settings_scene_start_bticon_changed,
        app);

    value_index = value_index_uint32(app->settings.bt_icon, bticon_value, DESKTOP_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, desktop_on_off_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "RPC Icon",
        DESKTOP_ON_OFF_COUNT,
        desktop_settings_scene_start_rpc_changed,
        app);

    value_index = value_index_uint32(app->settings.rpc_icon, rpc_value, DESKTOP_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, desktop_on_off_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "SD Card Icon",
        DESKTOP_ON_OFF_COUNT,
        desktop_settings_scene_start_sdcard_changed,
        app);

    value_index = value_index_uint32(app->settings.sdcard, sdcard_value, DESKTOP_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, desktop_on_off_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Stealth Icon",
        DESKTOP_ON_OFF_COUNT,
        desktop_settings_scene_start_stealth_changed,
        app);

    value_index =
        value_index_uint32(app->settings.stealth_icon, stealth_value, DESKTOP_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, desktop_on_off_text[value_index]);

    item = variable_item_list_add(
        variable_item_list,
        "Top Bar",
        DESKTOP_ON_OFF_COUNT,
        desktop_settings_scene_start_topbar_changed,
        app);

    value_index = value_index_uint32(app->settings.top_bar, topbar_value, DESKTOP_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, desktop_on_off_text[value_index]);

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

    item = variable_item_list_add(
        variable_item_list,
        "Games Only Icon",
        DESKTOP_ON_OFF_COUNT,
        desktop_settings_scene_start_dumbmode_icon_changed,
        app);

    value_index =
        value_index_uint32(app->settings.dumbmode_icon, dumbmode_icon_value, DESKTOP_ON_OFF_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, desktop_on_off_text[value_index]);

    variable_item_list_set_enter_callback(
        variable_item_list, desktop_settings_scene_start_var_list_enter_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewVarItemList);
}

bool desktop_settings_scene_start_on_event(void* context, SceneManagerEvent sme) {
    DesktopSettingsApp* app = context;
    bool consumed = false;

    if(sme.type == SceneManagerEventTypeCustom) {
        switch(sme.event) {
        case SCENE_EVENT_SELECT_FAVORITE_PRIMARY:
            scene_manager_set_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite, 0);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_FAVORITE_SECONDARY:
            scene_manager_set_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite, 1);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_FAVORITE_TERTIARY:
            scene_manager_set_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite, 2);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_FAVORITE_QUATERNARY:
            scene_manager_set_scene_state(app->scene_manager, DesktopSettingsAppSceneFavorite, 3);
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppSceneFavorite);
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_PIN_SETUP:
            scene_manager_next_scene(app->scene_manager, DesktopSettingsAppScenePinMenu);
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_AUTO_LOCK_DELAY:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_AUTO_LOCK_PIN:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_ICON_STYLE:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_BATTERY_DISPLAY:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_LOCK_ICON:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_BT_ICON:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_RPC_ICON:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_SDCARD_ICON:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_STEALTH_ICON:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_TOP_BAR:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_DUMBMODE:
            consumed = true;
            break;
        case SCENE_EVENT_SELECT_DUMBMODE_ICON:
            consumed = true;
            break;
        }
    }
    return consumed;
}

void desktop_settings_scene_start_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    variable_item_list_reset(app->variable_item_list);
    DESKTOP_SETTINGS_SAVE(&app->settings);

    if((app->settings.icon_style != origIconStyle_value) ||
       (app->settings.displayBatteryPercentage != origBattDisp_value)) {
        // Trigger UI update in case we changed battery layout
        Power* power = furi_record_open(RECORD_POWER);
        power_update_viewport(power);
        furi_record_close(RECORD_POWER);
    }
}
