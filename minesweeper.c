#include "minesweeper.h"

static bool minesweeper_custom_event_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = (App*)context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

static bool minesweeper_navigation_event_callback(void* context) {
    furi_assert(context);
    App* app = (App*)context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void minesweeper_tick_event_callback(void* context) {
    furi_assert(context);
    App* app = (App*)context;
    return scene_manager_handle_tick_event(app->scene_manager);
}

static App* app_alloc() { 
    App* app = (App*)malloc(sizeof(App));
    
    // Get Gui and NotificationApp
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    // Turn backlight on
    notification_message(app->notification, &sequence_display_backlight_on);

    // Alloc Scene Manager and set handlers for on_enter, on_event, on_exit 
    app->scene_manager = scene_manager_alloc(&minesweeper_scene_handlers, app);
    
    // Alloc View Dispatcher and enable queue
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    // Set View Dispatcher event callback context and callbacks
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, minesweeper_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, minesweeper_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(app->view_dispatcher, minesweeper_tick_event_callback, 500);

    // Alloc views and add to view dispatcher
    app->start_screen = start_screen_alloc();
    view_dispatcher_add_view(app->view_dispatcher, MineSweeperStartScreenView, start_screen_get_view(app->start_screen));

    app->loading = loading_alloc();
    view_dispatcher_add_view(app->view_dispatcher, MineSweeperLoadingView, loading_get_view(app->loading));

    app->menu = dialog_ex_alloc();
    view_dispatcher_add_view(app->view_dispatcher, MineSweeperMenuView, dialog_ex_get_view(app->menu));

    // Set hardware related values to default
    app->haptic = 1;
    app->speaker = 1;
    app->led = 1;

    return app;

}

static void app_free(App* app) {
    furi_assert(app);
    
    // Remove each view from View Dispatcher
    for (MineSweeperView minesweeper_view = (MineSweeperView)0; minesweeper_view < MineSweeperViewCount; minesweeper_view++) {
        view_dispatcher_remove_view(app->view_dispatcher, minesweeper_view);
    }

    // Free View Dispatcher and Scene Manager
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    // Free views
    loading_free(app->loading);
    dialog_ex_free(app->menu);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    app->gui = NULL;
    app->notification = NULL;

    // Free app structure
    free(app);

}

int32_t minesweeper_app(void* p) {
    UNUSED(p);

    App* app = app_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    // This will be the initial scene on app startup
    scene_manager_next_scene(app->scene_manager, MineSweeperSceneMenu);

    furi_hal_power_suppress_charge_enter();
    
    view_dispatcher_run(app->view_dispatcher);

    furi_hal_power_suppress_charge_exit();

    view_dispatcher_stop(app->view_dispatcher);
    scene_manager_stop(app->scene_manager);

    app_free(app);

    return 0;
}
