#include "color_guess.h"
#include "digits.h"

bool color_guess_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    ColorGuess* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void color_guess_tick_event_callback(void* context) {
    furi_assert(context);
    ColorGuess* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool color_guess_navigation_event_callback(void* context) {
    furi_assert(context);
    ColorGuess* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

ColorGuess* color_guess_app_alloc() {
    ColorGuess* app = malloc(sizeof(ColorGuess));
    //app->event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);
    //app->plugin_state = malloc(sizeof(PluginState));
    //app->view_port = view_port_alloc();
    app->error = false;

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notification, &sequence_display_backlight_on);

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&color_guess_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, color_guess_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, color_guess_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, color_guess_custom_event_callback);
    app->submenu = submenu_alloc();

    view_dispatcher_add_view(
        app->view_dispatcher, ColorGuessViewIdMenu, submenu_get_view(app->submenu));
    app->color_guess_color_set = color_guess_color_set_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        ColorGuessViewIdColorSet,
        color_guess_color_set_get_view(app->color_guess_color_set));
    app->color_guess_play = color_guess_play_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        ColorGuessViewIdPlay,
        color_guess_play_get_view(app->color_guess_play));

    //End Scene Additions

    return app;
}

void color_guess_app_free(ColorGuess* app) {
    furi_assert(app);

    // Scene manager
    scene_manager_free(app->scene_manager);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, ColorGuessViewIdMenu);
    submenu_free(app->submenu);

    view_dispatcher_free(app->view_dispatcher);

    // GUI
    furi_record_close(RECORD_GUI);

    app->event_queue = NULL;
    app->plugin_state = NULL;
    // app->state_mutex = NULL;
    app->view_port = NULL;

    app->gui = NULL;
    //furi_record_close(RECORD_NOTIFICATION);
    app->notification = NULL;

    //Remove whatever is left
    free(app);
}

int32_t color_guess_app(void* p) {
    UNUSED(p);
    ColorGuess* app = color_guess_app_alloc();
    if(app->error) {
        return 255;
    }

    if(!furi_hal_region_is_provisioned()) {
        color_guess_app_free(app);
        return 1;
    }

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(app->scene_manager, ColorGuessSceneStart);

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    furi_hal_power_suppress_charge_exit();

    color_guess_app_free(app);

    return 0;
}
