#include "desktop/desktop_helpers.h"
#include "furi/record.h"
#include <FreeRTOS.h>
#include <portmacro.h>
#include <timer.h>

#include "desktop/desktop_i.h"
#include "desktop/views/pin_timeout.h"
#include "gui/scene_manager.h"
#include "desktop_scene.h"
#include "desktop_scene_i.h"

#define SCENE_EVENT_EXIT 0

static void desktop_scene_pin_timeout_callback(void* context) {
    Desktop* desktop = (Desktop*)context;
    view_dispatcher_send_custom_event(desktop->view_dispatcher, SCENE_EVENT_EXIT);
}

void desktop_scene_pin_timeout_on_enter(void* context) {
    Desktop* desktop = (Desktop*)context;

    desktop_helpers_emit_error_notification();

    uint32_t timeout = scene_manager_get_scene_state(desktop->scene_manager,
            DesktopScenePinTimeout);
    desktop_view_pin_timeout_start(desktop->pin_timeout_view, timeout);
    desktop_view_pin_timeout_set_callback(desktop->pin_timeout_view, desktop_scene_pin_timeout_callback, desktop);

    view_dispatcher_switch_to_view(desktop->view_dispatcher, DesktopViewIdPinTimeout);
}

bool desktop_scene_pin_timeout_on_event(void* context, SceneManagerEvent event) {
    Desktop* desktop = (Desktop*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case SCENE_EVENT_EXIT:
            scene_manager_search_and_switch_to_another_scene(desktop->scene_manager, DesktopSceneLocked);
            consumed = true;
            break;
        }
    }

    return consumed;
}

void desktop_scene_pin_timeout_on_exit(void* context) {
}

