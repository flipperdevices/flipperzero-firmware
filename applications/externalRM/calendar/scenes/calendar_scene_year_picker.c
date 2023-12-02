#include "../calendar.h"
#include <furi_hal.h>
#include <gui/scene_manager.h>

void calendar_scene_year_picker_callback(CalendarAppCustomEvent event, void* context) {
    furi_assert(context);
    CalendarApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

bool calendar_scene_year_picker_on_event(void* context, SceneManagerEvent event) {
    CalendarApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case CalendarAppCustomEventYearPicked:
            scene_manager_next_scene(app->scene_manager, CalendarSceneMonthPicker);
            consumed = true;
            break;
        }
    }

    return consumed;
}

void calendar_scene_year_picker_on_enter(void* context) {
    CalendarApp* app = context;

    calendar_year_picker_set_callback(
        app->calendar_year_picker, calendar_scene_year_picker_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, CalendarAppViewYearPicker);
}

void calendar_scene_year_picker_on_exit(void* context) {
    UNUSED(context);
}