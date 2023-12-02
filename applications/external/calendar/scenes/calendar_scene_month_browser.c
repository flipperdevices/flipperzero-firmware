#include "../calendar.h"
#include <furi_hal.h>
#include <gui/scene_manager.h>

void calendar_scene_month_browser_callback(CalendarAppCustomEvent event, void* context) {
    furi_assert(context);
    CalendarApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

bool calendar_scene_month_browser_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);

    return false;
}

void calendar_scene_month_browser_on_enter(void* context) {
    CalendarApp* app = context;

    calendar_year_picker_set_callback(
        app->calendar_year_picker, calendar_scene_month_browser_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, CalendarAppViewMonthBrowser);
}

void calendar_scene_month_browser_on_exit(void* context) {
    UNUSED(context);
}