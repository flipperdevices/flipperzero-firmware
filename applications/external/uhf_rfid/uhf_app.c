#include "uhf_app_i.h"

bool uhf_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    UHFApp* uhf_app = context;
    return scene_manager_handle_custom_event(uhf_app->scene_manager, event);
}

bool uhf_back_event_callback(void* context) {
    furi_assert(context);
    UHFApp* uhf_app = context;
    return scene_manager_handle_back_event(uhf_app->scene_manager);
}

void uhf_tick_event_callback(void* context) {
    furi_assert(context);
    UHFApp* uhf_app = context;
    scene_manager_handle_tick_event(uhf_app->scene_manager);
}

UHFApp* uhf_app_alloc() {
    UHFApp* uhf_app = (UHFApp*)malloc(sizeof(UHFApp));
    uhf_app->worker = (UHFWorker*)uhf_worker_alloc();
    uhf_app->view_dispatcher = view_dispatcher_alloc();
    uhf_app->scene_manager = scene_manager_alloc(&uhf_scene_handlers, uhf_app);
    view_dispatcher_enable_queue(uhf_app->view_dispatcher);
    view_dispatcher_set_event_callback_context(uhf_app->view_dispatcher, uhf_app);
    view_dispatcher_set_custom_event_callback(uhf_app->view_dispatcher, uhf_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        uhf_app->view_dispatcher, uhf_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        uhf_app->view_dispatcher, uhf_tick_event_callback, 100);

    // Open GUI record
    uhf_app->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        uhf_app->view_dispatcher, uhf_app->gui, ViewDispatcherTypeFullscreen);

    // Open Notification record
    uhf_app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Submenu
    uhf_app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        uhf_app->view_dispatcher, UHFViewMenu, submenu_get_view(uhf_app->submenu));

    // Popup
    uhf_app->popup = popup_alloc();
    view_dispatcher_add_view(
        uhf_app->view_dispatcher, UHFViewPopup, popup_get_view(uhf_app->popup));

    // Loading
    uhf_app->loading = loading_alloc();
    view_dispatcher_add_view(
        uhf_app->view_dispatcher, UHFViewLoading, loading_get_view(uhf_app->loading));

    // Text Input
    uhf_app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        uhf_app->view_dispatcher, UHFViewTextInput, text_input_get_view(uhf_app->text_input));

    // Custom Widget
    uhf_app->widget = widget_alloc();
    view_dispatcher_add_view(
        uhf_app->view_dispatcher, UHFViewWidget, widget_get_view(uhf_app->widget));

    return uhf_app;
}

void uhf_app_free(UHFApp* uhf_app) {
    furi_assert(uhf_app);

    // Submenu
    view_dispatcher_remove_view(uhf_app->view_dispatcher, UHFViewMenu);
    submenu_free(uhf_app->submenu);

    // Popup
    view_dispatcher_remove_view(uhf_app->view_dispatcher, UHFViewPopup);
    popup_free(uhf_app->popup);

    // Loading
    view_dispatcher_remove_view(uhf_app->view_dispatcher, UHFViewLoading);
    loading_free(uhf_app->loading);

    // TextInput
    view_dispatcher_remove_view(uhf_app->view_dispatcher, UHFViewTextInput);
    text_input_free(uhf_app->text_input);

    // Custom Widget
    view_dispatcher_remove_view(uhf_app->view_dispatcher, UHFViewWidget);
    widget_free(uhf_app->widget);

    // Worker
    uhf_worker_stop(uhf_app->worker);
    uhf_worker_free(uhf_app->worker);

    // View Dispatcher
    view_dispatcher_free(uhf_app->view_dispatcher);

    // Scene Manager
    scene_manager_free(uhf_app->scene_manager);

    // GUI
    furi_record_close(RECORD_GUI);
    uhf_app->gui = NULL;

    // Notifications
    furi_record_close(RECORD_NOTIFICATION);
    uhf_app->notifications = NULL;

    free(uhf_app);
}

int32_t uhf_app_main(void* ctx) {
    UNUSED(ctx);
    UHFApp* uhf_app = uhf_app_alloc();
    view_dispatcher_attach_to_gui(
        uhf_app->view_dispatcher, uhf_app->gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(uhf_app->scene_manager, UHFSceneStart);
    view_dispatcher_run(uhf_app->view_dispatcher);

    uhf_app_free(uhf_app);
    return 0;
}