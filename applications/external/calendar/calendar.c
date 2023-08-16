#include "calendar.h"
#include <furi.h>
#include <core/check.h>
#include <core/record.h>
#include <core/log.h>
#include <core/log.h>
#include <furi_hal_rtc.h>

static bool calendar_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    CalendarApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool calendar_app_back_event_callback(void* context) {
    furi_assert(context);
    CalendarApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

VariableSharedContext* calendar_app_variable_shared_context_alloc() {
    FuriHalRtcDateTime datetime;
    furi_hal_rtc_get_datetime(&datetime);
    VariableSharedContext* variable_shared_context = malloc(sizeof(VariableSharedContext));
    variable_shared_context->year_selected = datetime.year;
    variable_shared_context->month_selected = datetime.month;

    return variable_shared_context;
}

CalendarApp* calendar_app_alloc() {
    CalendarApp* app = malloc(sizeof(CalendarApp));

    // Variable shared context
    app->variable_shared_context = calendar_app_variable_shared_context_alloc();

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();

    // Scene manager
    app->scene_manager = scene_manager_alloc(&calendar_scene_handlers, app);

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, calendar_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, calendar_app_back_event_callback);

    // Open GUI record
    app->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Views
    app->calendar_year_picker = calendar_year_picker_alloc(app->variable_shared_context);
    view_dispatcher_add_view(
        app->view_dispatcher,
        CalendarAppViewYearPicker,
        calendar_year_picker_get_view(app->calendar_year_picker));

    app->calendar_month_picker = calendar_month_picker_alloc(app->variable_shared_context);
    view_dispatcher_add_view(
        app->view_dispatcher,
        CalendarAppViewMonthPicker,
        calendar_month_picker_get_view(app->calendar_month_picker));

    app->calendar_month_browser = calendar_month_browser_alloc(app->variable_shared_context);
    view_dispatcher_add_view(
        app->view_dispatcher,
        CalendarAppViewMonthBrowser,
        calendar_month_browser_get_view(app->calendar_month_browser));

    scene_manager_next_scene(app->scene_manager, CalendarSceneYearPicker);

    return app;
}

void calendar_app_free(CalendarApp* app) {
    furi_assert(app);

    // Views
    view_dispatcher_remove_view(app->view_dispatcher, CalendarAppViewYearPicker);
    calendar_year_picker_free(app->calendar_year_picker);

    view_dispatcher_remove_view(app->view_dispatcher, CalendarAppViewMonthPicker);
    calendar_month_picker_free(app->calendar_month_picker);

    view_dispatcher_remove_view(app->view_dispatcher, CalendarAppViewMonthBrowser);
    calendar_month_browser_free(app->calendar_month_browser);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);

    // Scene manager
    scene_manager_free(app->scene_manager);

    // GUI
    furi_record_close(RECORD_GUI);
    app->gui = NULL;

    free(app);
}

int32_t calendar_app(void* p) {
    UNUSED(p);
    CalendarApp* app = calendar_app_alloc();

    view_dispatcher_run(app->view_dispatcher);

    calendar_app_free(app);

    return 0;
}
