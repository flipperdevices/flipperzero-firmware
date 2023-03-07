#include "flipbip.h"

bool flipbip_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    FlipBip* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void flipbip_tick_event_callback(void* context) {
    furi_assert(context);
    FlipBip* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool flipbip_navigation_event_callback(void* context) {
    furi_assert(context);
    FlipBip* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

FlipBip* flipbip_app_alloc() {
    FlipBip* app = malloc(sizeof(FlipBip));
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&flipbip_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, flipbip_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, flipbip_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, flipbip_custom_event_callback);
    app->submenu = submenu_alloc();

    // Settings
    app->haptic = 1;
    app->led = 1;
    app->bip39_strength = 2; // 256 bits (24 words)
    app->bip44_coin = 0; // 0 (BTC)
    app->overwrite_saved_seed = 0;

    view_dispatcher_add_view(
        app->view_dispatcher, FlipBipViewIdMenu, submenu_get_view(app->submenu));
    app->flipbip_startscreen = flipbip_startscreen_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipBipViewIdStartscreen,
        flipbip_startscreen_get_view(app->flipbip_startscreen));
    app->flipbip_scene_1 = flipbip_scene_1_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, FlipBipViewIdScene1, flipbip_scene_1_get_view(app->flipbip_scene_1));
    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipBipViewIdSettings,
        variable_item_list_get_view(app->variable_item_list));

    //End Scene Additions

    return app;
}

void flipbip_app_free(FlipBip* app) {
    furi_assert(app);

    // Scene manager
    scene_manager_free(app->scene_manager);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, FlipBipViewIdMenu);
    view_dispatcher_remove_view(app->view_dispatcher, FlipBipViewIdScene1);
    // view_dispatcher_remove_view(app->view_dispatcher, FlipBipViewIdScene2);
    view_dispatcher_remove_view(app->view_dispatcher, FlipBipViewIdSettings);
    submenu_free(app->submenu);

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    app->gui = NULL;
    app->notification = NULL;

    //Remove whatever is left
    free(app);
}

int32_t flipbip_app(void* p) {
    UNUSED(p);
    FlipBip* app = flipbip_app_alloc();

    // Disabled because causes exit on customer firmwares such as RM
    /*if(!furi_hal_region_is_provisioned()) {
        flipbip_app_free(app);
        return 1;
    }*/

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(
        app->scene_manager, FlipBipSceneStartscreen); //Start with start screen
    //scene_manager_next_scene(app->scene_manager, FlipBipSceneMenu); //if you want to directly start with Menu

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    furi_hal_power_suppress_charge_exit();
    flipbip_app_free(app);

    return 0;
}
