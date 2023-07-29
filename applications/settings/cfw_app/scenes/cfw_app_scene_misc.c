#include "../cfw_app.h"

enum VarItemListIndex {
    VarItemListIndexChangeDeviceName,
    VarItemListIndexChargeCap,
    VarItemListIndexRgbBacklight,
    VarItemListIndexLcdColor,
};

void cfw_app_scene_misc_var_item_list_callback(void* context, uint32_t index) {
    CfwApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

#define CHARGE_CAP_INTV 5
static void cfw_app_scene_misc_charge_cap_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    char cap_str[6];
    uint32_t value = (variable_item_get_current_value_index(item) + 1) * CHARGE_CAP_INTV;
    snprintf(cap_str, 6, "%lu%%", value);
    variable_item_set_current_value_text(item, cap_str);
    CFW_SETTINGS()->charge_cap = value;
    app->save_settings = true;
}

static void cfw_app_scene_misc_lcd_color_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rgb_backlight_get_color_text(index));
    rgb_backlight_set_color(index);
    app->save_backlight = true;
    notification_message(app->notification, &sequence_display_backlight_on);
}

void cfw_app_scene_misc_on_enter(void* context) {
    CfwApp* app = context;
    CfwSettings* cfw_settings = CFW_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    variable_item_list_add(var_item_list, "Change Device Name", 0, NULL, app);

    char cap_str[6];
    value_index = cfw_settings->charge_cap / CHARGE_CAP_INTV;
    snprintf(cap_str, 6, "%lu%%", (uint32_t)value_index * CHARGE_CAP_INTV);
    item = variable_item_list_add(
        var_item_list,
        "Charge Cap",
        100 / CHARGE_CAP_INTV,
        cfw_app_scene_misc_charge_cap_changed,
        app);
    variable_item_set_current_value_index(item, value_index - 1);
    variable_item_set_current_value_text(item, cap_str);

    item = variable_item_list_add(var_item_list, "RGB Backlight", 1, NULL, app);
    variable_item_set_current_value_text(item, cfw_settings->rgb_backlight ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "LCD Color",
        rgb_backlight_get_color_count(),
        cfw_app_scene_misc_lcd_color_changed,
        app);
    value_index = rgb_backlight_get_settings()->display_color_index;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, rgb_backlight_get_color_text(value_index));
    variable_item_set_locked(item, !cfw_settings->rgb_backlight, "Needs RGB\nBacklight!");

    variable_item_list_set_enter_callback(
        var_item_list, cfw_app_scene_misc_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, CfwAppSceneMisc));

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewVarItemList);
}

bool cfw_app_scene_misc_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, CfwAppSceneMisc, event.event);
        consumed = true;
        switch(event.event) {
        case VarItemListIndexChangeDeviceName:
            scene_manager_set_scene_state(app->scene_manager, CfwAppSceneMiscRename, 0);
            scene_manager_next_scene(app->scene_manager, CfwAppSceneMiscRename);
            break;
        case VarItemListIndexRgbBacklight: {
            bool change = CFW_SETTINGS()->rgb_backlight;
            if(!change) {
                DialogMessage* msg = dialog_message_alloc();
                dialog_message_set_header(msg, "RGB Backlight", 64, 0, AlignCenter, AlignTop);
                dialog_message_set_buttons(msg, "No", NULL, "Yes");
                dialog_message_set_text(
                    msg,
                    "This option requires installing\na hardware modification!\nIs it installed?",
                    64,
                    32,
                    AlignCenter,
                    AlignCenter);
                if(dialog_message_show(app->dialogs, msg) == DialogMessageButtonRight) {
                    change = true;
                }
                dialog_message_free(msg);
            }
            if(change) {
                CFW_SETTINGS()->rgb_backlight = !CFW_SETTINGS()->rgb_backlight;
                app->save_settings = true;
                notification_message(app->notification, &sequence_display_backlight_on);
                scene_manager_previous_scene(app->scene_manager);
                scene_manager_set_scene_state(app->scene_manager, CfwAppSceneMisc, 0);
                scene_manager_next_scene(app->scene_manager, CfwAppSceneMisc);
            }
            break;
        }
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_misc_on_exit(void* context) {
    CfwApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
