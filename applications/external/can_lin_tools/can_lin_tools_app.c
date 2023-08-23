#include "can_lin_tools_app_i.h"

#include <furi.h>
#include <furi_hal.h>

static bool can_lin_tools_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    CanLinToolsApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool can_lin_tools_app_back_event_callback(void* context) {
    furi_assert(context);
    CanLinToolsApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void can_lin_tools_app_tick_event_callback(void* context) {
    furi_assert(context);
    CanLinToolsApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

CanLinToolsApp* can_lin_tools_app_alloc() {
    CanLinToolsApp* app = malloc(sizeof(CanLinToolsApp));

    // GUI
    app->gui = furi_record_open(RECORD_GUI);

    // View Dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&can_lin_tools_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, can_lin_tools_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, can_lin_tools_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, can_lin_tools_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Open Notification record
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // SubMenu
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, CanLinToolsViewSubmenu, submenu_get_view(app->submenu));

    // Widget
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, CanLinToolsViewWidget, widget_get_view(app->widget));

    // Can Hacker 2
    app->can_lin_tools_can_hacker2_view = can_lin_tools_view_can_hacker2_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        CanLinToolsViewCanHacker2,
        can_lin_tools_view_can_hacker2_get_view(app->can_lin_tools_can_hacker2_view));

    // Lin Hacker
    app->can_lin_tools_lin_hacker_view = can_lin_tools_view_lin_hacker_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        CanLinToolsViewLinHacker,
        can_lin_tools_view_lin_hacker_get_view(app->can_lin_tools_lin_hacker_view));

    // CarBus Analyzer
    app->can_lin_tools_carbus_analyzer_view = can_lin_tools_view_carbus_analyzer_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        CanLinToolsViewCarBusAnalyzer,
        can_lin_tools_view_carbus_analyzer_get_view(app->can_lin_tools_carbus_analyzer_view));

    scene_manager_next_scene(app->scene_manager, CanLinToolsSceneStart);

    return app;
}

void can_lin_tools_app_free(CanLinToolsApp* app) {
    furi_assert(app);

    // Submenu
    view_dispatcher_remove_view(app->view_dispatcher, CanLinToolsViewSubmenu);
    submenu_free(app->submenu);

    //  Widget
    view_dispatcher_remove_view(app->view_dispatcher, CanLinToolsViewWidget);
    widget_free(app->widget);

    // Can Hacker 2
    view_dispatcher_remove_view(app->view_dispatcher, CanLinToolsViewCanHacker2);
    can_lin_tools_view_can_hacker2_free(app->can_lin_tools_can_hacker2_view);

    // Lin Hacker
    view_dispatcher_remove_view(app->view_dispatcher, CanLinToolsViewLinHacker);
    can_lin_tools_view_lin_hacker_free(app->can_lin_tools_lin_hacker_view);

    // CarBus Analyzer
    view_dispatcher_remove_view(app->view_dispatcher, CanLinToolsViewCarBusAnalyzer);
    can_lin_tools_view_carbus_analyzer_free(app->can_lin_tools_carbus_analyzer_view);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Notifications
    furi_record_close(RECORD_NOTIFICATION);
    app->notifications = NULL;

    // Close records
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t can_lin_tools_app(void* p) {
    UNUSED(p);
    CanLinToolsApp* can_lin_tools_app = can_lin_tools_app_alloc();

    view_dispatcher_run(can_lin_tools_app->view_dispatcher);

    can_lin_tools_app_free(can_lin_tools_app);

    return 0;
}
