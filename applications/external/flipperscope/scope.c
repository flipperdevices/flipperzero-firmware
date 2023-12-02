#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>

#include "scope_app_i.h"

void assert_failed(uint8_t* file, uint32_t line) {
    UNUSED(file);
    UNUSED(line);
    while(1) {
    }
}

static bool scope_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    ScopeApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool scope_app_back_event_callback(void* context) {
    furi_assert(context);
    ScopeApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void scope_app_tick_event_callback(void* context) {
    furi_assert(context);
    ScopeApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

ScopeApp* scope_app_alloc() {
    ScopeApp* app = malloc(sizeof(ScopeApp));

    // GUI
    app->gui = furi_record_open(RECORD_GUI);

    // View Dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&scope_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, scope_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, scope_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, scope_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // Open Notification record
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Variable Item List
    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        ScopeViewVariableItemList,
        variable_item_list_get_view(app->variable_item_list));

    // SubMenu
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, ScopeViewSubmenu, submenu_get_view(app->submenu));

    // Widget
    app->widget = widget_alloc();
    view_dispatcher_add_view(app->view_dispatcher, ScopeViewWidget, widget_get_view(app->widget));

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, ScopeViewSave, text_input_get_view(app->text_input));

    app->time = 0.001;
    app->measurement = m_time;

    scene_manager_next_scene(app->scene_manager, ScopeSceneStart);
    return app;
}

void scope_app_free(ScopeApp* app) {
    furi_assert(app);

    // Submenu
    view_dispatcher_remove_view(app->view_dispatcher, ScopeViewSubmenu);
    submenu_free(app->submenu);

    // Variable Item List
    view_dispatcher_remove_view(app->view_dispatcher, ScopeViewVariableItemList);
    variable_item_list_free(app->variable_item_list);

    // Text input
    view_dispatcher_remove_view(app->view_dispatcher, ScopeViewWidget);
    text_input_free(app->text_input);

    //  Widget
    view_dispatcher_remove_view(app->view_dispatcher, ScopeViewSave);
    widget_free(app->widget);

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

int32_t scope_main(void* p) {
    UNUSED(p);
    ScopeApp* scope_app = scope_app_alloc();
    view_dispatcher_run(scope_app->view_dispatcher);
    scope_app_free(scope_app);
    return 0;
}
