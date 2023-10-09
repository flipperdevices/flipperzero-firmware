#include "../cfw_app.h"

enum VarItemListIndex {
    VarItemListIndexDarkMode,
    VarItemListIndexLeftHanded,
    VarItemListIndexRgbBacklight,
    VarItemListIndexLcdStyle,
};

void cfw_app_scene_misc_screen_var_item_list_callback(void* context, uint32_t index) {
    CfwApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void cfw_app_scene_misc_screen_dark_mode_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    CFW_SETTINGS()->dark_mode = value;
    app->save_settings = true;
}

static void cfw_app_scene_misc_screen_hand_orient_changed(VariableItem* item) {
    bool value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    if(value) {
        furi_hal_rtc_set_flag(FuriHalRtcFlagHandOrient);
    } else {
        furi_hal_rtc_reset_flag(FuriHalRtcFlagHandOrient);
    }
}

const char* const lcd_styles[] = {
    "Static",
    "Custom",
    "Rainbow",
};

static void cfw_app_scene_misc_screen_lcd_style_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint32_t value = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, lcd_styles[value]);
    CFW_SETTINGS()->lcd_style = value;

    switch(value) {
    case 0:
    case 1:
        notification_message(app->notification, &sequence_display_backlight_off);
        rgb_backlight_set_rainbow_mode(0);
        rgb_backlight_set_color(0, &lcd_colors[0].color);
        rgb_backlight_set_color(1, &lcd_colors[0].color);
        rgb_backlight_set_color(2, &lcd_colors[0].color);
        notification_message(app->notification, &sequence_display_backlight_on);
        break;
    case 2:
    default:
        break;
    }
    app->save_backlight = true;
    app->save_settings = true;
}

void cfw_app_scene_misc_screen_on_enter(void* context) {
    CfwApp* app = context;
    CfwSettings* cfw_settings = CFW_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;

    item = variable_item_list_add(
        var_item_list, "Dark Mode", 2, cfw_app_scene_misc_screen_dark_mode_changed, app);
    variable_item_set_current_value_index(item, cfw_settings->dark_mode);
    variable_item_set_current_value_text(item, cfw_settings->dark_mode ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list, "Left Handed", 2, cfw_app_scene_misc_screen_hand_orient_changed, app);
    bool value = furi_hal_rtc_is_flag_set(FuriHalRtcFlagHandOrient);
    variable_item_set_current_value_index(item, value);
    variable_item_set_current_value_text(item, value ? "ON" : "OFF");

    item = variable_item_list_add(var_item_list, "RGB Backlight", 1, NULL, app);
    variable_item_set_current_value_text(item, cfw_settings->rgb_backlight ? "ON" : "OFF");

    item = variable_item_list_add(
        var_item_list,
        "LCD Style",
        COUNT_OF(lcd_styles),
        cfw_app_scene_misc_screen_lcd_style_changed,
        app);
    variable_item_set_current_value_index(item, cfw_settings->lcd_style);
    variable_item_set_current_value_text(item, lcd_styles[cfw_settings->lcd_style]);
    variable_item_set_locked(item, !cfw_settings->rgb_backlight, "Needs RGB\nBacklight!");

    variable_item_list_set_enter_callback(
        var_item_list, cfw_app_scene_misc_screen_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list, scene_manager_get_scene_state(app->scene_manager, CfwAppSceneMiscScreen));

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewVarItemList);
}

bool cfw_app_scene_misc_screen_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(app->scene_manager, CfwAppSceneMiscScreen, event.event);
        consumed = true;
        switch(event.event) {
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
                app->save_backlight = true;
                notification_message(app->notification, &sequence_display_backlight_on);
                rgb_backlight_reconfigure(CFW_SETTINGS()->rgb_backlight);
                scene_manager_next_scene(app->scene_manager, CfwAppSceneMiscScreen);
            }
            break;
        }
        case VarItemListIndexLcdStyle:
            scene_manager_set_scene_state(app->scene_manager, CfwAppSceneMiscScreenRGBSettings, 0);
            scene_manager_next_scene(app->scene_manager, CfwAppSceneMiscScreenRGBSettings);
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_misc_screen_on_exit(void* context) {
    CfwApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
