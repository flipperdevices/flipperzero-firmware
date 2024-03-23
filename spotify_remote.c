#include "spotify_remote.h"
#include "spotify_remote_i.h"

// handles data received from UART
static void uart_process_line(FuriString* line, void* context) {
    SpotifyRemoteApp* app = context;
    furi_string_set(app->message_recieved, line);
    scene_manager_handle_custom_event(app->scene_manager, SPOTIFY_REMOTE_ON_RECEIVE_EVENT);
}

// array of on_enter handlers
static void (*const spotify_remote_scene_on_enter_handlers[])(void*) = {
    spotify_remote_main_menu_scene_on_enter,
    spotify_remote_connecting_scene_on_enter,
    spotify_remote_ip_scene_on_enter,
    spotify_remote_button_panel_scene_on_enter,
    spotify_remote_about_scene_on_enter,
};

// array of on_event handlers
static bool (*const spotify_remote_scene_on_event_handlers[])(void*, SceneManagerEvent) = {
    spotify_remote_main_menu_scene_on_event,
    spotify_remote_connecting_scene_on_event,
    spotify_remote_ip_scene_on_event,
    spotify_remote_button_panel_scene_on_event,
    spotify_remote_about_scene_on_event,
};

// array of on_exit handlers
static void (*const spotify_remote_scene_on_exit_handlers[])(void*) = {
    spotify_remote_main_menu_scene_on_exit,
    spotify_remote_connecting_scene_on_exit,
    spotify_remote_ip_scene_on_exit,
    spotify_remote_button_panel_scene_on_exit,
    spotify_remote_about_scene_on_exit,
};

// create custom event callback
static bool spotify_remote_custom_callback(void* context, uint32_t custom_event) {
    SpotifyRemoteApp* app = context;

    // delegate custom event handling to scene manager
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

// create back event callback
static bool spotify_remote_back_event_callback(void* context) {
    SpotifyRemoteApp* app = context;

    if(app->is_remote_launched) {
        // if in remote view, back button goes back to main menu
        app->is_remote_launched = false;
        uart_helper_send(app->uart_helper, "8\n", 2);
        scene_manager_previous_scene(app->scene_manager);
        scene_manager_search_and_switch_to_another_scene(
            app->scene_manager, SPOTIFY_REMOTE_MAIN_MENU_SCENE);
        return true;
    } else {
        // delegate back event to scene manager
        return scene_manager_handle_back_event(app->scene_manager);
    }
}

// attach all handlers to the scene manager
static const SceneManagerHandlers spotify_remote_scene_manager_handlers = {
    .on_enter_handlers = spotify_remote_scene_on_enter_handlers,
    .on_event_handlers = spotify_remote_scene_on_event_handlers,
    .on_exit_handlers = spotify_remote_scene_on_exit_handlers,
    .scene_num = SPOTIFY_REMOTE_SCENE_COUNT,
};

// function to allocate memory and initialize the fields in the App struct
static SpotifyRemoteApp* spotify_remote_app_alloc() {
    // initialize app, scene manager, and view dispatcher
    SpotifyRemoteApp* app = malloc(sizeof(SpotifyRemoteApp));
    app->scene_manager = scene_manager_alloc(&spotify_remote_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();

    // enable view dispatcher queue to handle events
    view_dispatcher_enable_queue(app->view_dispatcher);

    // set callbacks and context for view dispacher
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, spotify_remote_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, spotify_remote_back_event_callback);

    // create and add views for modules
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, SPOTIFY_REMOTE_SUBMENU_VIEW, submenu_get_view(app->submenu));
    app->text_box = text_box_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, SPOTIFY_REMOTE_TEXT_BOX_VIEW, text_box_get_view(app->text_box));
    app->button_panel = button_panel_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        SPOTIFY_REMOTE_BUTTON_PANEL_VIEW,
        button_panel_get_view(app->button_panel));
    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        SPOTIFY_REMOTE_TEXT_INPUT_VIEW,
        text_input_get_view(app->text_input));

    // Initialize the UART helper.
    app->uart_helper = uart_helper_alloc();
    uart_helper_set_baud_rate(app->uart_helper, DEVICE_BAUDRATE);
    uart_helper_set_delimiter(app->uart_helper, LINE_DELIMITER, INCLUDE_LINE_DELIMITER);
    uart_helper_set_callback(app->uart_helper, uart_process_line, app);

    // allocate memory for wifi ssid and password
    app->wifi_ssid_size = 32; // max ssid size
    app->wifi_ssid = malloc(app->wifi_ssid_size);
    app->wifi_password_size = 63; // max password size
    app->wifi_password = malloc(app->wifi_password_size);

    app->test_str = furi_string_alloc();

    app->message_recieved = furi_string_alloc();

    app->is_remote_launched = false;

    return app;
}

// free all data
static void app_free(SpotifyRemoteApp* app) {
    view_dispatcher_remove_view(app->view_dispatcher, SPOTIFY_REMOTE_SUBMENU_VIEW);
    view_dispatcher_remove_view(app->view_dispatcher, SPOTIFY_REMOTE_TEXT_BOX_VIEW);
    view_dispatcher_remove_view(app->view_dispatcher, SPOTIFY_REMOTE_BUTTON_PANEL_VIEW);
    view_dispatcher_remove_view(app->view_dispatcher, SPOTIFY_REMOTE_TEXT_INPUT_VIEW);

    uart_helper_free(app->uart_helper);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    button_panel_free(app->button_panel);
    text_box_free(app->text_box);
    furi_string_free(app->message_recieved);
    furi_string_free(app->test_str);

    free(app->wifi_ssid);
    free(app->wifi_password);

    free(app);
}

int32_t spotify_remote_app(void* p) {
    UNUSED(p);

    SpotifyRemoteApp* app = spotify_remote_app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, SPOTIFY_REMOTE_MAIN_MENU_SCENE);
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);

    return 0;
}
