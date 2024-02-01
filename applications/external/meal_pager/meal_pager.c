#include "meal_pager.h"

bool meal_pager_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    Meal_Pager* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void meal_pager_tick_event_callback(void* context) {
    furi_assert(context);
    Meal_Pager* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool meal_pager_navigation_event_callback(void* context) {
    furi_assert(context);
    Meal_Pager* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

Meal_Pager* meal_pager_app_alloc() {
    Meal_Pager* app = malloc(sizeof(Meal_Pager));
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&meal_pager_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, meal_pager_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, meal_pager_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, meal_pager_custom_event_callback);
    app->submenu = submenu_alloc();

    // Set defaults, in case no config loaded
    app->haptic = 1;
    app->speaker = 1;
    app->led = 1;
    app->save_settings = 1;
    app->pager_type = 0;
    app->first_station = 0;
    app->first_station_char = "0";
    app->last_station = 10;
    app->last_station_char = "10";
    app->first_pager = 0;
    app->first_pager_char = "0";
    app->last_pager = 31;
    app->last_pager_char = "31";
    app->stop_transmit = false;
    app->repeats = 1;
    app->repeats_char = "1";
    app->max_station = 8191;
    app->max_pager = 999;

    snprintf(app->text_store[0], 32, "%lu", app->first_station);

    // Used for File Browser
    app->dialogs = furi_record_open(RECORD_DIALOGS);
    app->file_path = furi_string_alloc();

    app->subghz = subghz_alloc();

    // Custom made int keyboard
    app->int_input = int_input_alloc();

    // Load configs
    meal_pager_read_settings(app);

    view_dispatcher_add_view(
        app->view_dispatcher, Meal_PagerViewIdMenu, submenu_get_view(app->submenu));
    app->meal_pager_startscreen = meal_pager_startscreen_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        Meal_PagerViewIdStartscreen,
        meal_pager_startscreen_get_view(app->meal_pager_startscreen));
    app->meal_pager_transmit = meal_pager_transmit_alloc(app);
    view_dispatcher_add_view(
        app->view_dispatcher,
        Meal_PagerViewIdTransmit,
        meal_pager_transmit_get_view(app->meal_pager_transmit));

    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        Meal_PagerViewIdSettings,
        variable_item_list_get_view(app->variable_item_list));

    app->int_input = int_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, Meal_PagerViewIdIntInput, int_input_get_view(app->int_input));

    //End Scene Additions

    snprintf(app->text_store[0], 20, "%lu", app->first_station);
    snprintf(app->text_store[1], 20, "%lu", app->last_station);
    snprintf(app->text_store[2], 20, "%lu", app->first_pager);
    snprintf(app->text_store[3], 20, "%lu", app->last_pager);

    return app;
}

void meal_pager_app_free(Meal_Pager* app) {
    furi_assert(app);

    // Scene manager
    scene_manager_free(app->scene_manager);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, Meal_PagerViewIdMenu);
    view_dispatcher_remove_view(app->view_dispatcher, Meal_PagerViewIdTransmit);
    view_dispatcher_remove_view(app->view_dispatcher, Meal_PagerViewIdSettings);
    view_dispatcher_remove_view(app->view_dispatcher, Meal_PagerViewIdIntInput);
    submenu_free(app->submenu);
    int_input_free(app->int_input);

    view_dispatcher_free(app->view_dispatcher);

    variable_item_list_free(app->variable_item_list);
    meal_pager_transmit_free(app->meal_pager_transmit);
    meal_pager_startscreen_free(app->meal_pager_startscreen);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    app->gui = NULL;
    app->notification = NULL;

    // Close File Browser
    furi_record_close(RECORD_DIALOGS);
    furi_string_free(app->file_path);

    subghz_free(app->subghz);

    //Remove whatever is left
    free(app);
}

int32_t meal_pager_app(void* p) {
    UNUSED(p);
    FURI_LOG_D(TAG, "Started Meal Pager");

    Meal_Pager* app = meal_pager_app_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(
        app->scene_manager, Meal_PagerSceneStartscreen); //Start with start screen
    //scene_manager_next_scene(app->scene_manager, Meal_PagerSceneMenu); //if you want to directly start with Menu

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    meal_pager_save_settings(app);

    furi_hal_power_suppress_charge_exit();
    meal_pager_app_free(app);

    return 0;
}
