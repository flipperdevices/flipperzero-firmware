#include <furi.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include "flipp_pomodoro_scene.h"
#include "../flipp_pomodoro_app.h"
#include "../views/flipp_pomodoro_timer_view.h"

enum { SceneEventConusmed = true, SceneEventNotConusmed = false };

void flipp_pomodoro_scene_timer_sync_view_state(void* ctx) {
    furi_assert(ctx);

    FlippPomodoroApp* app = ctx;

    flipp_pomodoro_view_timer_set_state(
        flipp_pomodoro_view_timer_get_view(app->timer_view), app->state);
}

void flipp_pomodoro_scene_timer_on_next_stage(void* ctx) {
    furi_assert(ctx);

    FlippPomodoroApp* app = ctx;

    view_dispatcher_send_custom_event(app->view_dispatcher, FlippPomodoroAppCustomEventStageSkip);
}

void flipp_pomodoro_scene_timer_on_enter(void* ctx) {
    furi_assert(ctx);

    FlippPomodoroApp* app = ctx;

    if(flipp_pomodoro__is_stage_expired(app->state)) {
        flipp_pomodoro__destroy(app->state);
        app->state = flipp_pomodoro__new();
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, FlippPomodoroAppViewTimer);
    flipp_pomodoro_scene_timer_sync_view_state(app);
    flipp_pomodoro_view_timer_set_on_right_cb(
        app->timer_view, flipp_pomodoro_scene_timer_on_next_stage, app);
}

void flipp_pomodoro_scene_timer_handle_custom_event(
    FlippPomodoroApp* app,
    FlippPomodoroAppCustomEvent custom_event) {
    if(custom_event == FlippPomodoroAppCustomEventTimerTick &&
       flipp_pomodoro__is_stage_expired(app->state)) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, FlippPomodoroAppCustomEventStageComplete);
    }

    if(custom_event == FlippPomodoroAppCustomEventStateUpdated) {
        flipp_pomodoro_scene_timer_sync_view_state(app);
    }
}

bool flipp_pomodoro_scene_timer_on_event(void* ctx, SceneManagerEvent event) {
    furi_assert(ctx);
    FlippPomodoroApp* app = ctx;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        flipp_pomodoro_scene_timer_handle_custom_event(app, event.event);
        return SceneEventConusmed;
    case SceneManagerEventTypeBack:
        scene_manager_next_scene(app->scene_manager, FlippPomodoroSceneInfo);
        return SceneEventConusmed;
    default:
        break;
    }
    return SceneEventNotConusmed;
}

void flipp_pomodoro_scene_timer_on_exit(void* ctx) {
    UNUSED(ctx);
}