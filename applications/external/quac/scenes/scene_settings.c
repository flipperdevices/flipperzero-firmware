#include <furi.h>

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <toolbox/value_index.h>

#include "quac.h"
#include "scenes.h"
#include "scene_settings.h"
#include "../actions/action.h"
#include "../views/action_menu.h"
#include "../quac_settings.h"

#include <lib/toolbox/path.h>

typedef enum {
    SceneSettingsLayout,
    SceneSettingsIcons,
    SceneSettingsHeaders,
    SceneSettingsRFIDDuration,
    SceneSettingsNFCDuration,
    SceneSettingsSubGHzRepeat,
    SceneSettingsSubGHzExtAnt,
    SceneSettingsHidden,
    SceneSettingsAbout
} SceneSettingsIndex;

static const char* const layout_text[2] = {"Vert", "Horiz"};
static const uint32_t layout_value[2] = {QUAC_APP_PORTRAIT, QUAC_APP_LANDSCAPE};

static const char* const show_offon_text[2] = {"OFF", "ON"};
static const uint32_t show_offon_value[2] = {false, true};

#define V_DURATION_COUNT 8
static const char* const duration_text[V_DURATION_COUNT] = {
    "500 ms",
    "1 sec",
    "1.5 sec",
    "2 sec",
    "2.5 sec",
    "3 sec",
    "5 sec",
    "10 sec",
};
static const uint32_t duration_value[V_DURATION_COUNT] = {
    500,
    1000,
    1500,
    2000,
    2500,
    3000,
    5000,
    10000,
};

#define V_REPEAT_COUNT 9
static const char* const repeat_text[V_REPEAT_COUNT] = {
    "1",
    "2",
    "3",
    "5",
    "8",
    "10", // default
    "15",
    "20",
    "50"};
static const uint32_t repeat_value[V_REPEAT_COUNT] = {1, 2, 3, 5, 8, 10, 15, 20, 50};

static const char* const subghz_ext_text[2] = {"Disabled", "Enabled"};
static const uint32_t subghz_ext_value[2] = {false, true};

static void scene_settings_layout_changed(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, layout_text[index]);
    app->settings.layout = layout_value[index];
}

static void scene_settings_show_icons_changed(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, show_offon_text[index]);
    app->settings.show_icons = show_offon_value[index];
}

static void scene_settings_show_headers_changed(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, show_offon_text[index]);
    app->settings.show_headers = show_offon_value[index];
}

static void scene_settings_rfid_duration_changed(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, duration_text[index]);
    app->settings.rfid_duration = duration_value[index];
}

static void scene_settings_nfc_duration_changed(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, duration_text[index]);
    app->settings.nfc_duration = duration_value[index];
}

static void scene_settings_subghz_repeat_changed(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, repeat_text[index]);
    app->settings.subghz_repeat = repeat_value[index];
}

static void scene_settings_subghz_ext_changed(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, subghz_ext_text[index]);
    app->settings.subghz_use_ext_antenna = subghz_ext_value[index];
}

static void scene_settings_show_hidden_changed(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, show_offon_text[index]);
    app->settings.show_hidden = show_offon_value[index];
}

static void scene_settings_enter_callback(void* context, uint32_t index) {
    App* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

// For each scene, implement handler callbacks
void scene_settings_on_enter(void* context) {
    App* app = context;

    VariableItemList* vil = app->vil_settings;
    variable_item_list_reset(vil);

    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(vil, "Layout", 2, scene_settings_layout_changed, app);
    value_index = value_index_uint32(app->settings.layout, layout_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, layout_text[value_index]);

    item = variable_item_list_add(vil, "Show Icons", 2, scene_settings_show_icons_changed, app);
    value_index = value_index_uint32(app->settings.show_icons, show_offon_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, show_offon_text[value_index]);

    item =
        variable_item_list_add(vil, "Show Headers", 2, scene_settings_show_headers_changed, app);
    value_index = value_index_uint32(app->settings.show_headers, show_offon_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, show_offon_text[value_index]);

    item = variable_item_list_add(
        vil, "RFID Duration", V_DURATION_COUNT, scene_settings_rfid_duration_changed, app);
    value_index =
        value_index_uint32(app->settings.rfid_duration, duration_value, V_DURATION_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, duration_text[value_index]);

    item = variable_item_list_add(
        vil, "NFC Duration", V_DURATION_COUNT, scene_settings_nfc_duration_changed, app);
    value_index = value_index_uint32(app->settings.nfc_duration, duration_value, V_DURATION_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, duration_text[value_index]);

    item = variable_item_list_add(
        vil, "SubGHz Repeat", V_REPEAT_COUNT, scene_settings_subghz_repeat_changed, app);
    value_index = value_index_uint32(app->settings.subghz_repeat, repeat_value, V_REPEAT_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, repeat_text[value_index]);

    item =
        variable_item_list_add(vil, "SubGHz Ext Ant", 2, scene_settings_subghz_ext_changed, app);
    value_index = value_index_uint32(app->settings.subghz_use_ext_antenna, subghz_ext_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, subghz_ext_text[value_index]);

    item = variable_item_list_add(vil, "Show Hidden", 2, scene_settings_show_hidden_changed, app);
    value_index = value_index_uint32(app->settings.show_hidden, show_offon_value, 2);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, show_offon_text[value_index]);

    // Last item is always "About"
    item = variable_item_list_add(vil, "About", 1, NULL, NULL);
    variable_item_list_set_enter_callback(vil, scene_settings_enter_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, QView_Settings);
}

bool scene_settings_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case SceneSettingsAbout:
            consumed = true;
            scene_manager_next_scene(app->scene_manager, QScene_About);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void scene_settings_on_exit(void* context) {
    App* app = context;
    VariableItemList* vil = app->vil_settings;
    variable_item_list_reset(vil);

    quac_save_settings(app);
}