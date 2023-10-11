#include "../cfw_app.h"

enum VarItemListIndex {
    VarItemListIndexLcdStaticColor,
    VarItemListIndexLcdColor0,
    VarItemListIndexLcdColor1,
    VarItemListIndexLcdColor2,
    VarItemListIndexRainbowLcd,
    VarItemListIndexRainbowSpeed,
    VarItemListIndexRainbowInterval,
    VarItemListIndexRainbowSaturation,
};

void cfw_app_scene_misc_screen_rgb_settings_var_item_list_callback(void* context, uint32_t index) {
    CfwApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

static void cfw_app_scene_misc_screen_lcd_static_color_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, lcd_colors[index].name);
    notification_message(app->notification, &sequence_display_backlight_off);
    rgb_backlight_set_color(0, &lcd_colors[index].color);
    rgb_backlight_set_color(1, &lcd_colors[index].color);
    rgb_backlight_set_color(2, &lcd_colors[index].color);
    notification_message(app->notification, &sequence_display_backlight_on);
    app->save_backlight = true;
}
static void cfw_app_scene_misc_screen_lcd_color_changed(VariableItem* item, uint8_t led) {
    CfwApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, lcd_colors[index].name);
    notification_message(app->notification, &sequence_display_backlight_off);
    rgb_backlight_set_color(led, &lcd_colors[index].color);
    notification_message(app->notification, &sequence_display_backlight_on);
    app->save_backlight = true;
}
static void cfw_app_scene_misc_screen_lcd_color_0_changed(VariableItem* item) {
    cfw_app_scene_misc_screen_lcd_color_changed(item, 0);
}
static void cfw_app_scene_misc_screen_lcd_color_1_changed(VariableItem* item) {
    cfw_app_scene_misc_screen_lcd_color_changed(item, 1);
}
static void cfw_app_scene_misc_screen_lcd_color_2_changed(VariableItem* item) {
    cfw_app_scene_misc_screen_lcd_color_changed(item, 2);
}
const char* const rainbow_lcd_names[RGBBacklightRainbowModeCount] = {
    "OFF",
    "Wave",
    "Static",
};
static void cfw_app_scene_misc_screen_rainbow_lcd_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rainbow_lcd_names[index]);
    rgb_backlight_set_rainbow_mode(index);
    app->save_backlight = true;
}
static void cfw_app_scene_misc_screen_rainbow_speed_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item) + 1;
    char str[4];
    snprintf(str, sizeof(str), "%d", index);
    variable_item_set_current_value_text(item, str);
    rgb_backlight_set_rainbow_speed(index);
    app->save_backlight = true;
}
const char* const rainbow_interval_names[] = {
    "0.1 S",
    "0.2 S",
    "0.25 S",
    "0.5 S",
    "0.75 S",
    "1 S",
    "1.25 S",
    "1.5 S",
    "1.75 S",
    "2 S",
    "2.5 S",
    "3 S",
    "4 S",
    "5 S",
};
const uint32_t rainbow_interval_values[COUNT_OF(rainbow_interval_names)] = {
    100,
    200,
    250,
    500,
    750,
    1000,
    1250,
    1500,
    1750,
    2000,
    2500,
    3000,
    4000,
    5000,
};
static void cfw_app_scene_misc_screen_rainbow_interval_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rainbow_interval_names[index]);
    rgb_backlight_set_rainbow_interval(rainbow_interval_values[index]);
    app->save_backlight = true;
}
static void cfw_app_scene_misc_screen_rainbow_saturation_changed(VariableItem* item) {
    CfwApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item) + 1;
    char str[4];
    snprintf(str, sizeof(str), "%d", index);
    variable_item_set_current_value_text(item, str);
    rgb_backlight_set_rainbow_saturation(index);
    app->save_backlight = true;
}

void cfw_app_scene_misc_screen_rgb_settings_on_enter(void* context) {
    CfwApp* app = context;
    CfwSettings* cfw_settings = CFW_SETTINGS();
    VariableItemList* var_item_list = app->var_item_list;
    VariableItem* item;
    uint8_t value_index;

    struct {
        uint8_t led;
        VariableItemChangeCallback cb;
    } lcd_cols[] = {
        {0, cfw_app_scene_misc_screen_lcd_color_0_changed},
        {1, cfw_app_scene_misc_screen_lcd_color_1_changed},
        {2, cfw_app_scene_misc_screen_lcd_color_2_changed},
    };

    size_t lcd_sz = COUNT_OF(lcd_colors);

    RgbColor color;
    switch(cfw_settings->lcd_style) {
    case 0:
        item = variable_item_list_add(
            var_item_list,
            "LCD Color",
            lcd_sz,
            cfw_app_scene_misc_screen_lcd_static_color_changed,
            app);
        rgb_backlight_get_color(0, &color);
        bool found = false;
        for(size_t i = 0; i < lcd_sz; i++) {
            if(rgbcmp(&color, &lcd_colors[i].color) != 0) continue;
            value_index = i;
            found = true;
            break;
        }

        variable_item_set_current_value_index(item, found ? value_index : lcd_sz);
        if(found) {
            variable_item_set_current_value_text(item, lcd_colors[value_index].name);
        } else {
            char str[7];
            snprintf(str, sizeof(str), "%02X%02X%02X", color.r, color.g, color.b);
            variable_item_set_current_value_text(item, str);
        }
        break;
    case 1:
        for(size_t i = 0; i < COUNT_OF(lcd_cols); i++) {
            char name[12];
            snprintf(name, sizeof(name), "LCD LED %u", lcd_cols[i].led + 1);
            item = variable_item_list_add(var_item_list, name, lcd_sz, lcd_cols[i].cb, app);
            rgb_backlight_get_color(lcd_cols[i].led, &color);
            bool found = false;
            for(size_t i = 0; i < lcd_sz; i++) {
                if(rgbcmp(&color, &lcd_colors[i].color) != 0) continue;
                value_index = i;
                found = true;
                break;
            }
            variable_item_set_current_value_index(item, found ? value_index : lcd_sz);
            if(found) {
                variable_item_set_current_value_text(item, lcd_colors[value_index].name);
            } else {
                char str[7];
                snprintf(str, sizeof(str), "%02X%02X%02X", color.r, color.g, color.b);
                variable_item_set_current_value_text(item, str);
            }
        }
        break;
    case 2:
        item = variable_item_list_add(
            var_item_list,
            "Rainbow LCD",
            RGBBacklightRainbowModeCount,
            cfw_app_scene_misc_screen_rainbow_lcd_changed,
            app);
        value_index = rgb_backlight_get_rainbow_mode();
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, rainbow_lcd_names[value_index]);

        item = variable_item_list_add(
            var_item_list,
            "Rainbow Speed",
            25,
            cfw_app_scene_misc_screen_rainbow_speed_changed,
            app);
        value_index = rgb_backlight_get_rainbow_speed();
        variable_item_set_current_value_index(item, value_index - 1);
        char speed_str[4];
        snprintf(speed_str, sizeof(speed_str), "%d", value_index);
        variable_item_set_current_value_text(item, speed_str);

        item = variable_item_list_add(
            var_item_list,
            "Rainbow Interval",
            COUNT_OF(rainbow_interval_values),
            cfw_app_scene_misc_screen_rainbow_interval_changed,
            app);
        value_index = value_index_uint32(
            rgb_backlight_get_rainbow_interval(),
            rainbow_interval_values,
            COUNT_OF(rainbow_interval_values));
        variable_item_set_current_value_index(item, value_index);
        variable_item_set_current_value_text(item, rainbow_interval_names[value_index]);

        item = variable_item_list_add(
            var_item_list,
            "Rainbow Saturation",
            255,
            cfw_app_scene_misc_screen_rainbow_saturation_changed,
            app);
        value_index = rgb_backlight_get_rainbow_saturation();
        variable_item_set_current_value_index(item, value_index - 1);
        char saturation_str[4];
        snprintf(saturation_str, sizeof(saturation_str), "%d", value_index);
        variable_item_set_current_value_text(item, saturation_str);
        break;
    default:
        break;
    }

    variable_item_list_set_enter_callback(
        var_item_list, cfw_app_scene_misc_screen_rgb_settings_var_item_list_callback, app);

    variable_item_list_set_selected_item(
        var_item_list,
        scene_manager_get_scene_state(app->scene_manager, CfwAppSceneMiscScreenRGBSettings));

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewVarItemList);
}

bool cfw_app_scene_misc_screen_rgb_settings_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    CfwSettings* cfw_settings = CFW_SETTINGS();
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case VarItemListIndexLcdStaticColor:
        case VarItemListIndexLcdColor0:
        case VarItemListIndexLcdColor1:
        case VarItemListIndexLcdColor2:
            if(cfw_settings->lcd_style == 0 || cfw_settings->lcd_style == 1) {
                scene_manager_set_scene_state(
                    app->scene_manager,
                    CfwAppSceneMiscScreenColor,
                    event.event - VarItemListIndexLcdStaticColor);
                scene_manager_next_scene(app->scene_manager, CfwAppSceneMiscScreenColor);
            }
            break;
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_misc_screen_rgb_settings_on_exit(void* context) {
    CfwApp* app = context;
    variable_item_list_reset(app->var_item_list);
}
