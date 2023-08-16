#include "../calendar.h"
#include <furi_hal.h>
#include <gui/scene_manager.h>

void calendar_scene_month_picker_callback(CalendarAppCustomEvent event, void* context) {
    furi_assert(context);
    CalendarApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

bool calendar_scene_month_picker_on_event(void* context, SceneManagerEvent event) {
    CalendarApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case CalendarAppCustomEventMontPicked:
            scene_manager_next_scene(app->scene_manager, CalendarSceneMonthBrowser);
            consumed = true;
            break;
        }
    }

    return consumed;
}

void calendar_scene_month_picker_on_enter(void* context) {
    CalendarApp* app = context;

    calendar_month_picker_set_callback(
        app->calendar_month_picker, calendar_scene_month_picker_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, CalendarAppViewMonthPicker);
}

void calendar_scene_month_picker_on_exit(void* context) {
    UNUSED(context);
}