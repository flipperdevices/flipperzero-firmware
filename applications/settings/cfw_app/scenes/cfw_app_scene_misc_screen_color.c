#include "../cfw_app.h"

enum ByteInputResult {
    ByteInputResultOk,
};

void cfw_app_scene_misc_screen_color_byte_input_callback(void* context) {
    CfwApp* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, ByteInputResultOk);
}

void cfw_app_scene_misc_screen_color_on_enter(void* context) {
    CfwApp* app = context;
    ByteInput* byte_input = app->byte_input;

    byte_input_set_header_text(byte_input, "Set LCD Color (#RRGGBB)");

    rgb_backlight_get_color(
        scene_manager_get_scene_state(app->scene_manager, CfwAppSceneMiscScreenColor),
        &app->lcd_color);

    byte_input_set_result_callback(
        byte_input,
        cfw_app_scene_misc_screen_color_byte_input_callback,
        NULL,
        app,
        (void*)&app->lcd_color,
        sizeof(app->lcd_color));

    view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewByteInput);
}

bool cfw_app_scene_misc_screen_color_on_event(void* context, SceneManagerEvent event) {
    CfwApp* app = context;
    CfwSettings* cfw_settings = CFW_SETTINGS();
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case ByteInputResultOk:
            switch(cfw_settings->lcd_style) {
            case 0:
                notification_message(app->notification, &sequence_display_backlight_off);
                rgb_backlight_set_color(0, &app->lcd_color);
                rgb_backlight_set_color(1, &app->lcd_color);
                rgb_backlight_set_color(2, &app->lcd_color);
                notification_message(app->notification, &sequence_display_backlight_on);
                app->save_backlight = true;
                break;
            case 1:
                notification_message(app->notification, &sequence_display_backlight_off);
                rgb_backlight_set_color(
                    scene_manager_get_scene_state(app->scene_manager, CfwAppSceneMiscScreenColor),
                    &app->lcd_color);
                notification_message(app->notification, &sequence_display_backlight_on);
                app->save_backlight = true;
                break;
            }
            scene_manager_previous_scene(app->scene_manager);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void cfw_app_scene_misc_screen_color_on_exit(void* context) {
    CfwApp* app = context;
    byte_input_set_result_callback(app->byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(app->byte_input, "");
}
