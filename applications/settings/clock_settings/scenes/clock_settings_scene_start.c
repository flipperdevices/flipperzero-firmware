#include "../clock_settings.h"
#include <furi_hal.h>

#define TAG "SceneStart"

typedef enum {
    SubmenuIndexPwm,
    SubmenuIndexClockOutput,
} SubmenuIndex;

void clock_settings_scene_start_submenu_callback(void* context, uint32_t index) {
    ClockSettings* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void clock_settings_scene_start_on_enter(void* context) {
    ClockSettings* app = context;

    view_dispatcher_switch_to_view(app->view_dispatcher, ClockSettingsViewPwm);
}

bool clock_settings_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);

    return false;
}

void clock_settings_scene_start_on_exit(void* context) {
    UNUSED(context);
}
