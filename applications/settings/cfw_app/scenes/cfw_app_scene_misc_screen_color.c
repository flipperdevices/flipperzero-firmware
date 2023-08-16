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

    app->lcd_color = rgb_backlight_get_color(
        scene_manager_get_scene_state(app->scene_manager, CfwAppSceneMiscScreenColor));

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
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case ByteInputResultOk:
            rgb_backlight_set_color(
                scene_manager_get_scene_state(app->scene_manager, CfwAppSceneMiscScreenColor),
                app->lcd_color);
            app->save_backlight = true;
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
