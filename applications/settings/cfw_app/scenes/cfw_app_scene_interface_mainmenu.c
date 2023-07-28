#include "../cfw_app.h"

enum VarItemListIndex {
    VarItemListIndexMenuStyle,
    VarItemListIndexStartPoint,
    VarItemListIndexResetMenu,
    VarItemListIndexMenuApp,
    VarItemListIndexMoveApp,
    VarItemListIndexAddApp,
    VarItemListIndexRemoveApp,
};

void cfw_app_scene_interface_mainmenu_var_item_list_callback(void* context, uint32_t index) {
    CfwApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

const char* const menu_style_names[MenuStyleCount] = {
    "List",
    "Wii",
    "DSi",
    "Vertical",
};
static void cfw_app_scene_interface_mainmenu_menu_style_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, menu_style_names[index]);
    CFW_SETTINGS()->menu_style = index;
    app->save_settings = true;
}

static void cfw_app_scene_interface_mainmenu_start_point_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    app->start_point_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(
        item, *CharList_get(app->mainmenu_app_names, app->start_point_index));
    CFW_SETTINGS()->start_point = app->start_point_index;
    app->save_settings = true;
    app->require_reboot = true;
}

static void cfw_app_scene_interface_mainmenu_menu_app_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    app->mainmenu_app_index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(
        item, *CharList_get(app->mainmenu_app_names, app->mainmenu_app_index));
    char label[21];
    snprintf(
        label,
        21,
        "Order %u/%u",
        1 + app->mainmenu_app_index,
        CharList_size(app->mainmenu_app_names));
    variable_item_set_item_label(item, label);
}

static void cfw_app_scene_interface_mainmenu_move_app_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint8_t idx = app->mainmenu_app_index;
    size_t size = CharList_size(app->mainmenu_app_names);
    uint8_t dir = variable_item_get_current_value_index(item);
    if(size >= 2) {
        if(dir == 2 && idx != size - 1) {
            // Right
            CharList_swap_at(app->mainmenu_app_names, idx, idx + 1);
            CharList_swap_at(app->mainmenu_app_paths, idx, idx + 1);
            app->mainmenu_app_index++;
        } else if(dir == 0 && idx != 0) {
            // Left
            CharList_swap_at(app->mainmenu_app_names, idx, idx - 1);
            CharList_swap_at(app->mainmenu_app_paths, idx, idx - 1);
            app->mainmenu_app_index--;
        }
        view_dispatcher_send_custom_event(app->view_dispatcher, VarItemListIndexMoveApp);
    }
    variable_item_set_current_value_index(item, 1);
}

void cfw_app_scene_interface_mainmenu_on_enter(void* context) {
    CfwApp* app = context;
    CfwSettings* cfw_settings = CFW_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list,
        "Style",
        MenuStyleCount,
        cfw_app_scene_interface_mainmenu_menu_style_changed,
        app);
    variable_item_set_current_value_index(item, cfw_settings->menu_style);
    variable_item_set_current_value_text(item, menu_style_names[cfw_settings->menu_style]);

    item = variable_item_list_add(
        var_item_list,
        "Start Point",
        CharList_size(app->mainmenu_app_names),
        cfw_app_scene_interface_mainmenu_start_point_changed,
        app);
    if((cfw_settings->start_point) &&
       (cfw_settings->start_point < CharList_size(app->mainmenu_app_names))) {
        app->start_point_index = cfw_settings->start_point;
    } else {
        app->start_point_index = 0;
    }
    variable_item_set_current_value_index(item, app->start_point_index);
    if(CharList_size(app->mainmenu_app_names)) {
        variable_item_set_current_value_text(
            item, *CharList_get(app->mainmenu_app_names, app->start_point_index));
    } else {
        variable_item_set_current_value_text(item, "None");
    }

    variable_item_list_add(var_item_list, "Reset Menu", 0, NULL, app);

    size_t count = CharList_size(app->mainmenu_app_names);
    item = variable_item_list_add(
        var_item_list, "Menu Apps", count, cfw_app_scene_interface_mainmenu_menu_app_changed, app);
    if(count) {
        app->mainmenu_app_index = CLAMP(app->mainmenu_app_index, count - 1, 0U);
        char label[21];
        snprintf(label, 21, "Order %u/%u", 1 + app->mainmenu_app_index, count);
        variable_item_set_item_label(item, label);
        variable_item_set_current_value_text(
            item, *CharList_get(app->mainmenu_app_names, app->mainmenu_app_index));
    } else {
        app->mainmenu_app_index = 0;
        variable_item_set_current_value_text(item, "None");
    }
    variable_item_set_current_value_index(item, app->mainmenu_app_index);

    item = variable_item_list_add(
        var_item_list, "Move App", 3, cfw_app_scene_interface_mainmenu_move_app_changed, app);
    variable_item_set_current_value_text(item, "");
    variable_item_set_current_value_index(item, 1);

    variable_item_list_add(var_item_list, "Add App", 0, NULL, app);

    item = variable_item_list_add(var_item_list, "Remove App", 0, NULL, app);
    variable_item_set_locked(item, count < 3, "Minimum of\n2 apps\nare required!");

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
        case VarItemListIndexResetMenu:
            scene_manager_next_scene(app->scene_manager, CfwAppSceneInterfaceMainmenuReset);
            break;
        case VarItemListIndexRemoveApp:
            if(!CharList_size(app->mainmenu_app_names)) break;
            if(!CharList_size(app->mainmenu_app_paths)) break;
            free(*CharList_get(app->mainmenu_app_names, app->mainmenu_app_index));
            free(*CharList_get(app->mainmenu_app_paths, app->mainmenu_app_index));
            CharList_remove_v(
                app->mainmenu_app_names, app->mainmenu_app_index, app->mainmenu_app_index + 1);
            CharList_remove_v(
                app->mainmenu_app_paths, app->mainmenu_app_index, app->mainmenu_app_index + 1);
            if(app->mainmenu_app_index) app->mainmenu_app_index--;
            /* fall through */
        case VarItemListIndexMoveApp:
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
