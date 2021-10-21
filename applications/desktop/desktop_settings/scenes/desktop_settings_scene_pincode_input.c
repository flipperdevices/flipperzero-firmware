#include "../desktop_settings_app.h"

#define SCENE_EXIT_EVENT (0U)

void desktop_settings_scene_code_input_callback(void* context) {
    DesktopSettingsApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, SCENE_EXIT_EVENT);
}

void desktop_settings_scene_pincode_input_on_enter(void* context) {
    DesktopSettingsApp* app = context;

    CodeInput* code_input = app->code_input;
    code_input_set_header_text(code_input, "Code Input");
    code_input_set_result_callback(
        code_input,
        desktop_settings_scene_code_input_callback,
        NULL,
        app,
        app->settings.pincode.data,
        10);

    view_dispatcher_switch_to_view(app->view_dispatcher, DesktopSettingsAppViewPincodeInput);
}

bool desktop_settings_scene_pincode_input_on_event(void* context, SceneManagerEvent event) {
    DesktopSettingsApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case SCENE_EXIT_EVENT:
            scene_manager_previous_scene(app->scene_manager);
            consumed = true;
            break;

        default:
            consumed = true;
            break;
        }
    }
    return consumed;
}

void desktop_settings_scene_pincode_input_on_exit(void* context) {
    DesktopSettingsApp* app = context;
    code_input_set_result_callback(app->code_input, NULL, NULL, NULL, NULL, 0);
    code_input_set_header_text(app->code_input, "");
}
