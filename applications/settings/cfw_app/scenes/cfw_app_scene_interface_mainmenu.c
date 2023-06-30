#include "../cfw_app.h"

#define START_POINT_COUNT 15
const char* const start_point_text[START_POINT_COUNT] = {
    "Clock",
    "Sub-GHz",
    "Sub-GHz Remote",
    "Sub-GHz Playlist",
    "125 kHz RFID",
    "NFC",
    "Infrared",
    "IR Remote",
    "GPIO",
    "iButton",
    "Bad USB",
    "U2F",
    "CFW Settings",
    "Settings",
    "Applications"};
const uint32_t start_point_value[START_POINT_COUNT] =
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};

enum VarItemListIndex {
    VarItemListIndexWiiMenu,
    VarItemListIndexStartPoint,
    VarItemListIndexApp,
    VarItemListIndexRemoveApp,
    VarItemListIndexAddApp,
};

void cfw_app_scene_interface_mainmenu_var_item_list_callback(void* context, uint32_t index) {
    CfwApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void cfw_app_scene_interface_mainmenu_wii_menu_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "Wii Grid" : "App List");
    CFW_SETTINGS()->wii_menu = value;
    app->save_settings = true;
}

static void cfw_app_scene_interface_mainmenu_start_point_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint32_t value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, start_point_text[value]);
    CFW_SETTINGS()->start_point = value;
    app->save_settings = true;
}

static void cfw_app_scene_interface_mainmenu_app_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    app->mainmenu_app_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(
        item, *CharList_get(app->mainmenu_app_names, app->mainmenu_app_index));
}

void cfw_app_scene_interface_mainmenu_on_enter(void* context) {
    CfwApp* app = context;
    CfwSettings* cfw_settings = CFW_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint32_t value_index;

    item = variable_item_list_add(
        var_item_list, "Style", 2, cfw_app_scene_interface_mainmenu_wii_menu_changed, app);
    variable_item_set_current_value_index(item, cfw_settings->wii_menu);
    variable_item_set_current_value_text(item, cfw_settings->wii_menu ? "Wii Grid" : "App List");

    item = variable_item_list_add(
        var_item_list,
        "Start Point",
        START_POINT_COUNT,
        cfw_app_scene_interface_mainmenu_start_point_changed,
        app);

    value_index =
        value_index_uint32(cfw_settings->start_point, start_point_value, START_POINT_COUNT);
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, start_point_text[value_index]);

    item = variable_item_list_add(
        var_item_list,
        "App",
        CharList_size(app->mainmenu_app_names),
        cfw_app_scene_interface_mainmenu_app_changed,
        app);
    app->mainmenu_app_index = 0;
    variable_item_set_current_value_index(item, app->mainmenu_app_index);
    if(CharList_size(app->mainmenu_app_names)) {
        variable_item_set_current_value_text(
            item, *CharList_get(app->mainmenu_app_names, app->mainmenu_app_index));
    } else {
        variable_item_set_current_value_text(item, "None");
    }

    variable_item_list_add(var_item_list, "Remove App", 0, NULL, app);

    variable_item_list_add(var_item_list, "Add App", 0, NULL, app);

    variable_item_list_set_enter_callback(
        var_item_list, cfw_app_scene_interface_mainmenu_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, CfwAppSceneInterfaceMainmenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewVarItemList);
}

bool cfw_app_scene_interface_mainmenu_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, CfwAppSceneInterfaceMainmenu, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexRemoveApp:
            if(!CharList_size(app->mainmenu_app_names)) break;
            if(!CharList_size(app->mainmenu_app_paths)) break;
            CharList_remove_v(
                app->mainmenu_app_names, app->mainmenu_app_index, app->mainmenu_app_index + 1);
            CharList_remove_v(
                app->mainmenu_app_paths, app->mainmenu_app_index, app->mainmenu_app_index + 1);
            app->save_mainmenu_apps = true;
            app->require_reboot = true;
            scene_manager_previous_scene(app->scene_manager);
            scene_manager_next_scene(app->scene_manager, CfwAppSceneInterfaceMainmenu);
            break;
        case VarItemListIndexAddApp:
            scene_manager_next_scene(app->scene_manager, CfwAppSceneInterfaceMainmenuAdd);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_interface_mainmenu_on_exit(void* context) {
    CfwApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
