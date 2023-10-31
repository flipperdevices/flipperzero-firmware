#include "flipp_pomodoro_app_i.h"

#define TAG "FlippPomodoro"

enum {
    CustomEventConsumed = true,
    CustomEventNotConsumed = false,
};

static bool flipp_pomodoro_app_back_event_callback(void* ctx) {
    furi_assert(ctx);
    FlippPomodoroApp* app = ctx;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void flipp_pomodoro_app_tick_event_callback(void* ctx) {
    furi_assert(ctx);
    FlippPomodoroApp* app = ctx;

    scene_manager_handle_custom_event(app->scene_manager, FlippPomodoroAppCustomEventTimerTick);
}

static bool flipp_pomodoro_app_custom_event_callback(void* ctx, uint32_t event) {
    furi_assert(ctx);
    FlippPomodoroApp* app = ctx;

    switch(event) {
    case FlippPomodoroAppCustomEventStageSkip:
        flipp_pomodoro__toggle_stage(app->state);
        view_dispatcher_send_custom_event(
            app->view_dispatcher, FlippPomodoroAppCustomEventStateUpdated);
        return CustomEventConsumed;
    case FlippPomodoroAppCustomEventStageComplete:
        if(flipp_pomodoro__get_stage(app->state) == FlippPomodoroStageFocus) {
            // REGISTER a deed on work stage complete to get an acheivement
            dolphin_deed(DolphinDeedPluginGameWin);
            FURI_LOG_I(TAG, "Focus stage reward added");

            flipp_pomodoro_statistics__increase_focus_stages_completed(app->statistics);
        };

        flipp_pomodoro__toggle_stage(app->state);
        notification_message(
            app->notification_app,
            stage_start_notification_sequence_map[flipp_pomodoro__get_stage(app->state)]);
        view_dispatcher_send_custom_event(
            app->view_dispatcher, FlippPomodoroAppCustomEventStateUpdated);
        return CustomEventConsumed;
    default:
        break;
    }
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

FlippPomodoroApp* flipp_pomodoro_app_alloc() {
    FlippPomodoroApp* app = malloc(sizeof(FlippPomodoroApp));
    app->state = flipp_pomodoro__new();

    app->scene_manager = scene_manager_alloc(&flipp_pomodoro_scene_handlers, app);
    app->gui = furi_record_open(RECORD_GUI);
    app->notification_app = furi_record_open(RECORD_NOTIFICATION);

    app->view_dispatcher = view_dispatcher_alloc();
    app->statistics = flipp_pomodoro_statistics__new();

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, flipp_pomodoro_app_custom_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, flipp_pomodoro_app_tick_event_callback, 1000);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, flipp_pomodoro_app_back_event_callback);

    app->timer_view = flipp_pomodoro_view_timer_alloc();
    app->info_view = flipp_pomodoro_info_view_alloc();

    view_dispatcher_add_view(
        app->view_dispatcher,
        FlippPomodoroAppViewTimer,
        flipp_pomodoro_view_timer_get_view(app->timer_view));

    view_dispatcher_add_view(
        app->view_dispatcher,
        FlippPomodoroAppViewInfo,
        flipp_pomodoro_info_view_get_view(app->info_view));

    scene_manager_next_scene(app->scene_manager, FlippPomodoroSceneTimer);
    FURI_LOG_I(TAG, "Alloc complete");
    return app;
}

void flipp_pomodoro_app_free(FlippPomodoroApp* app) {
    view_dispatcher_remove_view(app->view_dispatcher, FlippPomodoroAppViewTimer);
    view_dispatcher_remove_view(app->view_dispatcher, FlippPomodoroAppViewInfo);
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);
    flipp_pomodoro_view_timer_free(app->timer_view);
    flipp_pomodoro_info_view_free(app->info_view);
    flipp_pomodoro_statistics__destroy(app->statistics);
    flipp_pomodoro__destroy(app->state);
    free(app);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
}

int32_t flipp_pomodoro_app(void* p) {
    UNUSED(p);
    FURI_LOG_I(TAG, "Initial");
    FlippPomodoroApp* app = flipp_pomodoro_app_alloc();

    FURI_LOG_I(TAG, "Run deed added");

    view_dispatcher_run(app->view_dispatcher);

    flipp_pomodoro_app_free(app);

    return 0;
}