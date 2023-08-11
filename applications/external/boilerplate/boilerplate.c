#include "boilerplate.h"

bool boilerplate_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    Boilerplate* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void boilerplate_tick_event_callback(void* context) {
    furi_assert(context);
    Boilerplate* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool boilerplate_navigation_event_callback(void* context) {
    furi_assert(context);
    Boilerplate* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

Boilerplate* boilerplate_app_alloc() {
    Boilerplate* app = malloc(sizeof(Boilerplate));
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&boilerplate_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, boilerplate_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, boilerplate_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, boilerplate_custom_event_callback);
    app->submenu = submenu_alloc();

    // Set defaults, in case no config loaded
    app->haptic = 1;
    app->speaker = 1;
    app->led = 1;
    app->save_settings = 1;

    // Used for File Browser
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->file_path = furi_string_alloc();

    // Load configs
    boilerplate_read_settings(app);

    view_dispatcher_add_view(
        app->view_dispatcher, BoilerplateViewIdMenu, submenu_get_view(app->submenu));
    app->boilerplate_startscreen = boilerplate_startscreen_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        BoilerplateViewIdStartscreen,
        boilerplate_startscreen_get_view(app->boilerplate_startscreen));
    app->boilerplate_scene_1 = boilerplate_scene_1_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        BoilerplateViewIdScene1,
        boilerplate_scene_1_get_view(app->boilerplate_scene_1));
    app->boilerplate_scene_2 = boilerplate_scene_2_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        BoilerplateViewIdScene2,
        boilerplate_scene_2_get_view(app->boilerplate_scene_2));
    app->button_menu = button_menu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, BoilerplateViewIdScene3, button_menu_get_view(app->button_menu));

    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        BoilerplateViewIdSettings,
        variable_item_list_get_view(app->variable_item_list));

    //End Scene Additions

    return app;
}

void boilerplate_app_free(Boilerplate* app) {
    furi_assert(app);

    // Scene manager
    scene_manager_free(app->scene_manager);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, BoilerplateViewIdMenu);
    view_dispatcher_remove_view(app->view_dispatcher, BoilerplateViewIdScene1);
    view_dispatcher_remove_view(app->view_dispatcher, BoilerplateViewIdScene2);
    view_dispatcher_remove_view(app->view_dispatcher, BoilerplateViewIdSettings);
    submenu_free(app->submenu);

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    app->gui = NULL;
    app->notification = NULL;

    // Close File Browser
    furi_record_close(RECORD_DIALOGS);
    furi_string_free(app->file_path);

    //Remove whatever is left
    free(app);
}

int32_t boilerplate_app(void* p) {
    UNUSED(p);
    Boilerplate* app = boilerplate_app_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(
        app->scene_manager, BoilerplateSceneStartscreen); //Start with start screen
    //scene_manager_next_scene(app->scene_manager, BoilerplateSceneMenu); //if you want to directly start with Menu

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    boilerplate_save_settings(app);

    furi_hal_power_suppress_charge_exit();
    boilerplate_app_free(app);

    return 0;
}
