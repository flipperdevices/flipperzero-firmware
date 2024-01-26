#include "flippertag.h"

bool flippertag_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    FlipperTag* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void flippertag_tick_event_callback(void* context) {
    furi_assert(context);
    FlipperTag* app = context;
    scene_manager_handle_tick_event(app->scene_manager);

    app->last_shield_time += 100;
    if(app->is_shielded && app->last_shield_time >= 1000) {
        app->shield_status -= 1;
        app->last_shield_time = 0;
    } else if(!app->is_shielded && app->last_shield_time >= 1000 && app->shield_status < 10) {
        app->shield_status += 1;
        app->last_shield_time = 0;
    }

    if(app->shield_status <= 0 && app->is_shielded) {
        app->is_shielded = false;
    }
}

//leave app if back button pressed
bool flippertag_navigation_event_callback(void* context) {
    furi_assert(context);
    FlipperTag* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

const uint32_t SoloMessage[17] =
    {2875, 6060, 2892, 2078, 888, 2086, 906, 2075, 905, 2074, 906, 2075, 905, 2081, 886, 2087, 905};

const uint32_t Team1Message[17] =
    {2889, 6053, 2874, 2086, 905, 2079, 888, 2092, 888, 2092, 887, 2092, 1882, 2084, 906, 2074, 906};

const uint32_t Team2Message[17] =
    {2849, 6086, 2890, 2081, 911, 2064, 926, 2058, 884, 2091, 1896, 2081, 883, 2090, 903, 2084, 883};

void flippertag_recieved_infrared_signal_callback(void* context, InfraredWorkerSignal* signal) {
    furi_assert(context);
    furi_assert(signal);
    FlipperTag* app = context;

    if(app->is_shielded) {
        return;
    }

    if(infrared_worker_signal_is_decoded(signal)) {
        return;
    }

    size_t size;
    const uint32_t* timings;
    infrared_worker_get_raw_signal(signal, &timings, &size);

    switch(app->team) {
    case FlipperTagSolo:
        if(timings != SoloMessage) {
            app->health -= 1;

            if(app->health > 0) {
                // flippertag_play_sound(app, 0);
                flippertag_led_set_rgb(app, 255, 0, 0);
                flippertag_play_long_bump(app);
                flippertag_led_reset(app);
            } else {
                flippertag_led_set_rgb(app, 255, 0, 0);
            }
        }
        break;
    case FlipperTagTeam1:
        if(timings == Team2Message) {
            app->health -= 1;

            if(app->health > 0) {
                // flippertag_play_sound(app, 0);
                flippertag_led_set_rgb(app, 255, 0, 0);
                flippertag_play_long_bump(app);
                flippertag_led_reset(app);
            } else {
                flippertag_led_set_rgb(app, 255, 0, 0);
            }
        }
        break;
    case FlipperTagTeam2:
        if(timings == Team1Message) {
            app->health -= 1;

            if(app->health > 0) {
                // flippertag_play_sound(app, 0);
                flippertag_led_set_rgb(app, 255, 0, 0);
                flippertag_play_long_bump(app);
                flippertag_led_reset(app);
            } else {
                flippertag_led_set_rgb(app, 255, 0, 0);
            }
        }
        break;
    }
}

void flippertag_get_signal_sent_infrared_callback(void* context) {
    furi_assert(context);
    FlipperTag* app = context;

    app->is_transmitting = false;
}

InfraredWorkerGetSignalResponse
    flippertag_get_signal_infrared_callback(void* context, InfraredWorker* instance) {
    UNUSED(instance);
    furi_assert(context);
    const FlipperTag* app = context;

    switch(app->team) {
    case FlipperTagSolo:
        infrared_worker_set_raw_signal(instance, SoloMessage, 17, 38000, 0.33);
        break;
    case FlipperTagTeam1:
        infrared_worker_set_raw_signal(instance, Team1Message, 17, 38000, 0.33);
        break;
    case FlipperTagTeam2:
        infrared_worker_set_raw_signal(instance, Team2Message, 17, 38000, 0.33);
        break;
    }

    return InfraredWorkerGetSignalResponseNew;
}

FlipperTag* FlipperTag_app_alloc() {
    FlipperTag* app = malloc(sizeof(FlipperTag));

    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);

    //Turn backlight on, believe me this makes testing your app easier
    notification_message(app->notification, &sequence_display_backlight_on);

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&FlipperTag_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, flippertag_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, flippertag_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, flippertag_custom_event_callback);
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
    flippertag_read_settings(app);

    app->flippertag_startscreen = flippertag_startscreen_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipperTagViewIdStartscreen,
        flippertag_startscreen_get_view(app->flippertag_startscreen));
    app->flippertag_scene_1 = flippertag_scene_1_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        FlipperTagViewIdScene1,
        flippertag_scene_1_get_view(app->flippertag_scene_1));

    //End Scene Additions

    // FlipperTag start
    app->team = FlipperTagSolo;
    app->health = DEFAULT_HEALTH;
    app->ammo = DEFAULT_AMMO;
    app->last_shield_time = 0;
    app->is_shielded = false;
    app->shield_status = DEFAULT_SHIELD;

    app->tx = infrared_worker_alloc();
    infrared_worker_tx_set_get_signal_callback(
        app->tx, flippertag_get_signal_infrared_callback, app);
    infrared_worker_tx_set_signal_sent_callback(
        app->tx, flippertag_get_signal_sent_infrared_callback, app);

    app->rx = infrared_worker_alloc();
    infrared_worker_rx_enable_signal_decoding(app->rx, false);
    infrared_worker_rx_enable_blink_on_receiving(app->rx, false);
    infrared_worker_rx_set_received_signal_callback(
        app->rx, flippertag_recieved_infrared_signal_callback, app);

    return app;
}

void flippertag_app_free(FlipperTag* app) {
    furi_assert(app);

    // Scene manager
    scene_manager_free(app->scene_manager);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, FlipperTagViewIdScene1);
    submenu_free(app->submenu);

    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    app->gui = NULL;
    app->notification = NULL;

    // Close File Browser
    furi_record_close(RECORD_DIALOGS);
    furi_string_free(app->file_path);

    // infrared_worker_rx_stop(app->rx);
    // infrared_worker_tx_stop(app->tx);

    infrared_worker_free(app->rx);
    infrared_worker_free(app->tx);

    //Remove whatever is left
    free(app);
}

int32_t flippertag_app(void* p) {
    UNUSED(p);
    FlipperTag* app = FlipperTag_app_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(
        app->scene_manager, FlipperTagSceneStartscreen); //Start with start screen
    //scene_manager_next_scene(app->scene_manager, FlipperTagSceneMenu); //if you want to directly start with Menu

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    flippertag_save_settings(app);

    furi_hal_power_suppress_charge_exit();
    flippertag_app_free(app);

    return 0;
}
