#include "minesweeper.h"
//#include <args.h>

static bool minesweeper_custom_event_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    MineSweeperApp* app = (MineSweeperApp*)context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

static bool minesweeper_navigation_event_callback(void* context) {
    furi_assert(context);
    MineSweeperApp* app = (MineSweeperApp*)context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void minesweeper_tick_event_callback(void* context) {
    furi_assert(context);
    MineSweeperApp* app = (MineSweeperApp*)context;
    return scene_manager_handle_tick_event(app->scene_manager);
}

static MineSweeperApp* app_alloc() {
    MineSweeperApp* app = (MineSweeperApp*)malloc(sizeof(MineSweeperApp));

    // NotificationApp Service
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    // Turn backlight on when app starts
    notification_message(app->notification, &sequence_display_backlight_on);

    // Alloc Scene Manager and set handlers for on_enter, on_event, on_exit
    app->scene_manager = scene_manager_alloc(&minesweeper_scene_handlers, app);

    // Alloc View Dispatcher and enable queue
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    // Set View Dispatcher event callback context and callbacks
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, minesweeper_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, minesweeper_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, minesweeper_tick_event_callback, 500);

    // Set setting info to default
    app->settings_info.width_str = furi_string_alloc();
    app->settings_info.height_str = furi_string_alloc();
    memset(&app->t_settings_info, 0, sizeof(app->t_settings_info));
    app->is_settings_changed = false;

    // If we cannot read the save file set to default values
    if(!(mine_sweeper_read_settings(app))) {
        FURI_LOG_I(TAG, "Cannot read save file, loading defaults");
        app->settings_info.board_width = 16;
        app->settings_info.board_height = 7;
        app->settings_info.difficulty = 0;
        app->haptic = 1;
        app->speaker = 1;
        app->led = 1;

        mine_sweeper_save_settings(app);
    } else {
        FURI_LOG_I(TAG, "Save file loaded sucessfully");
    }

    // Alloc views and add to view dispatcher
    app->start_screen = start_screen_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        MineSweeperStartScreenView,
        start_screen_get_view(app->start_screen));

    app->loading = loading_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MineSweeperLoadingView, loading_get_view(app->loading));

    app->game_screen = mine_sweeper_game_screen_alloc(
        app->settings_info.board_width,
        app->settings_info.board_height,
        app->settings_info.difficulty,
        false);

    view_dispatcher_add_view(
        app->view_dispatcher,
        MineSweeperGameScreenView,
        mine_sweeper_game_screen_get_view(app->game_screen));

    app->menu_screen = dialog_ex_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MineSweeperMenuView, dialog_ex_get_view(app->menu_screen));

    app->settings_screen = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        MineSweeperSettingsView,
        variable_item_list_get_view(app->settings_screen));

    app->confirmation_screen = dialog_ex_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        MineSweeperConfirmationView,
        dialog_ex_get_view(app->confirmation_screen));

    app->info_screen = text_box_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, MineSweeperInfoView, text_box_get_view(app->info_screen));

    Gui* gui = furi_record_open(RECORD_GUI);

    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    furi_record_close(RECORD_GUI);

    return app;
}

static void app_free(MineSweeperApp* app) {
    furi_assert(app);

    // Remove each view from View Dispatcher
    for(MineSweeperView minesweeper_view = (MineSweeperView)0;
        minesweeper_view < MineSweeperViewCount;
        minesweeper_view++) {
        view_dispatcher_remove_view(app->view_dispatcher, minesweeper_view);
    }

    // Free View Dispatcher and Scene Manager
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    // Free views
    loading_free(app->loading);
    start_screen_free(app->start_screen);
    mine_sweeper_game_screen_free(app->game_screen);
    dialog_ex_free(app->menu_screen);
    variable_item_list_free(app->settings_screen);
    dialog_ex_free(app->confirmation_screen);
    text_box_free(app->info_screen);

    furi_string_free(app->settings_info.width_str);
    furi_string_free(app->settings_info.height_str);

    furi_record_close(RECORD_NOTIFICATION);

    // Free app structure
    free(app);
}

int32_t minesweeper_app(void* p) {
    UNUSED(p);

    MineSweeperApp* app = app_alloc();
    FURI_LOG_D(TAG, "Mine Sweeper app allocated with size : %d", sizeof(*app));

    // This will be the initial scene on app startup
    scene_manager_next_scene(app->scene_manager, MineSweeperSceneStartScreen);

    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    FURI_LOG_D(TAG, "Mine Sweeper app freed");

    return 0;
}
