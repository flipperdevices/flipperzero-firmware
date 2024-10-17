#include "../clock_settings.h"
#include <furi_hal.h>

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

    DateTime dt;
    bool enabled = furi_hal_rtc_get_alarm(&dt);
    furi_hal_rtc_set_alarm(NULL);
    FURI_LOG_D("ClockSettings", "OnEnter: %u:%u enabled=%u", dt.hour, dt.minute, enabled);
    clock_settings_module_set(app->pwm_view, &dt, enabled);

    view_dispatcher_switch_to_view(app->view_dispatcher, ClockSettingsViewPwm);
}

bool clock_settings_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
    }

    return consumed;
}

void clock_settings_scene_start_on_exit(void* context) {
    ClockSettings* app = context;

    DateTime dt;
    bool enabled = clock_settings_module_get(app->pwm_view, &dt);
    FURI_LOG_D("ClockSettings", "OnExit: %u:%u enabled=%u", dt.hour, dt.minute, enabled);
    furi_hal_rtc_set_alarm(enabled ? &dt : NULL);
}
