#include "flipbip39.h"

bool flipbip39_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    FlipBip39* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void flipbip39_tick_event_callback(void* context) {
    furi_assert(context);
    FlipBip39* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool flipbip39_navigation_event_callback(void* context) {
    furi_assert(context);
    FlipBip39* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

FlipBip39* flipbip39_app_alloc() {
    FlipBip39* app = malloc(sizeof(FlipBip39));
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);
    
    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&flipbip39_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, flipbip39_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(app->view_dispatcher, flipbip39_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, flipbip39_custom_event_callback);
    app->submenu = submenu_alloc();

    app->haptic = 1;
    app->speaker = 1;
    app->led = 1;

    view_dispatcher_add_view(app->view_dispatcher, FlipBip39ViewIdMenu, submenu_get_view(app->submenu));
    app->flipbip39_startscreen = flipbip39_startscreen_alloc();
    view_dispatcher_add_view(app->view_dispatcher, FlipBip39ViewIdStartscreen, flipbip39_startscreen_get_view(app->flipbip39_startscreen));
    app->flipbip39_scene_1 = flipbip39_scene_1_alloc();
    view_dispatcher_add_view(app->view_dispatcher, FlipBip39ViewIdScene1, flipbip39_scene_1_get_view(app->flipbip39_scene_1));
    app->flipbip39_scene_2 = flipbip39_scene_2_alloc();
    view_dispatcher_add_view(app->view_dispatcher, FlipBip39ViewIdScene2, flipbip39_scene_2_get_view(app->flipbip39_scene_2));
    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(app->view_dispatcher, FlipBip39ViewIdSettings, variable_item_list_get_view(app->variable_item_list));

    //End Scene Additions

    return app;
}

void flipbip39_app_free(FlipBip39* app) {
    furi_assert(app);
    
    // Scene manager
    scene_manager_free(app->scene_manager);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, FlipBip39ViewIdMenu);
    view_dispatcher_remove_view(app->view_dispatcher, FlipBip39ViewIdScene1);
    view_dispatcher_remove_view(app->view_dispatcher, FlipBip39ViewIdScene2);
    view_dispatcher_remove_view(app->view_dispatcher, FlipBip39ViewIdSettings);
    submenu_free(app->submenu);

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);
    
    app->gui = NULL;
    app->notification = NULL;

    //Remove whatever is left
    free(app);
}

int32_t flipbip39_app(void* p) {
    UNUSED(p);
    FlipBip39* app = flipbip39_app_alloc();
    
    // Disabled because causes exit on customer firmwares such as RM
    /*if(!furi_hal_region_is_provisioned()) {
        flipbip39_app_free(app);
        return 1;
    }*/
    
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    
    scene_manager_next_scene(app->scene_manager, FlipBip39SceneStartscreen); //Start with start screen
    //scene_manager_next_scene(app->scene_manager, FlipBip39SceneMenu); //if you want to directly start with Menu

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);
    
    furi_hal_power_suppress_charge_exit();
    flipbip39_app_free(app);

    return 0;
}



