#include "dtmf_dolphin_i.h"

#include <furi.h>
#include <furi_hal.h>

static bool dtmf_dolphin_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    DTMFDolphinApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool dtmf_dolphin_app_back_event_callback(void* context) {
    furi_assert(context);
    DTMFDolphinApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void dtmf_dolphin_app_tick_event_callback(void* context) {
    furi_assert(context);
    DTMFDolphinApp* app = context;

    // Needed to handle queueing to ISR and prioritization of audio
    if (app->player.playing) {
        dtmf_dolphin_player_handle_tick();
    } else {
        scene_manager_handle_tick_event(app->scene_manager);
    }
}

static DTMFDolphinApp* app_alloc() {
    DTMFDolphinApp* app = malloc(sizeof(DTMFDolphinApp));
    app->player.half_samples = 4 * 1024;
    app->player.sample_count = 8 * 1024;
    app->player.sample_buffer = malloc(sizeof(uint16_t) * app->player.sample_count);
    app->player.buffer_buffer = malloc(sizeof(uint8_t) * app->player.sample_count);
    app->player.wf1_period = 0;
    app->player.wf2_period = 0;
    app->player.wf1_freq = 0;
    app->player.wf2_freq = 0;
    app->player.wf1_pos = 0;
    app->player.wf2_pos = 0;
    app->player.queue = furi_message_queue_alloc(10, sizeof(DTMFDolphinEvent));
    app->player.volume = 2.0f;
    app->player.playing = false;

    app->gui = furi_record_open(RECORD_GUI);
    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&dtmf_dolphin_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, dtmf_dolphin_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, dtmf_dolphin_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, dtmf_dolphin_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->main_menu_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        DTMFDolphinViewMainMenu,
        variable_item_list_get_view(app->main_menu_list));

    app->dtmf_dolphin_dialer = dtmf_dolphin_dialer_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        DTMFDolphinViewDialer,
        dtmf_dolphin_dialer_get_view(app->dtmf_dolphin_dialer));

    app->dtmf_dolphin_bluebox = dtmf_dolphin_bluebox_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        DTMFDolphinViewBluebox,
        dtmf_dolphin_bluebox_get_view(app->dtmf_dolphin_bluebox));

    app->dtmf_dolphin_play = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        DTMFDolphinViewPlay,
        widget_get_view(app->dtmf_dolphin_play));

    // app->dialer_button_panel = button_panel_alloc();
    // app->bluebox_button_panel = button_panel_alloc();
    // app->redbox_button_panel = button_panel_alloc();

    app->notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(app->notification, &sequence_display_backlight_enforce_on);

    scene_manager_next_scene(app->scene_manager, DTMFDolphinSceneStart);

    return app;
}

static void app_free(DTMFDolphinApp* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, DTMFDolphinViewMainMenu);
    view_dispatcher_remove_view(app->view_dispatcher, DTMFDolphinViewBluebox);
    view_dispatcher_remove_view(app->view_dispatcher, DTMFDolphinViewDialer);
    view_dispatcher_remove_view(app->view_dispatcher, DTMFDolphinViewPlay);
    variable_item_list_free(app->main_menu_list);

    dtmf_dolphin_bluebox_free(app->dtmf_dolphin_bluebox);
    dtmf_dolphin_dialer_free(app->dtmf_dolphin_dialer);
    widget_free(app->dtmf_dolphin_play);

    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    furi_message_queue_free(app->player.queue);
    free(app->player.sample_buffer);

    // button_panel_free(app->dialer_button_panel);
    // button_panel_free(app->bluebox_button_panel);
    // button_panel_free(app->redbox_button_panel);

    notification_message(app->notification, &sequence_display_backlight_enforce_auto);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    free(app);
}

int32_t dtmf_dolphin_app(void *p) {
    UNUSED(p);
    DTMFDolphinApp* app = app_alloc();

    dtmf_dolphin_player_init(&(app->player));
    
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}