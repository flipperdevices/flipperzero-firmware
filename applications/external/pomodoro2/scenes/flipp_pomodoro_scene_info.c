#include <furi.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include "flipp_pomodoro_scene.h"
#include "../flipp_pomodoro_app.h"
#include "../views/flipp_pomodoro_info_view.h"

enum { SceneEventConusmed = true, SceneEventNotConusmed = false };

void flipp_pomodoro_scene_info_on_back_to_timer(void* ctx) {
    furi_assert(ctx);
    FlippPomodoroApp* app = ctx;

    view_dispatcher_send_custom_event(
        app->view_dispatcher, FlippPomodoroAppCustomEventResumeTimer);
}

void flipp_pomodoro_scene_info_on_enter(void* ctx) {
    furi_assert(ctx);
    FlippPomodoroApp* app = ctx;

    view_dispatcher_switch_to_view(app->view_dispatcher, FlippPomodoroAppViewInfo);
    flipp_pomodoro_info_view_set_pomodoros_completed(
        flipp_pomodoro_info_view_get_view(app->info_view),
        flipp_pomodoro_statistics__get_focus_stages_completed(app->statistics));
    flipp_pomodoro_info_view_set_mode(
        flipp_pomodoro_info_view_get_view(app->info_view), FlippPomodoroInfoViewModeStats);
    flipp_pomodoro_info_view_set_resume_timer_cb(
        app->info_view, flipp_pomodoro_scene_info_on_back_to_timer, app);
}

void flipp_pomodoro_scene_info_handle_custom_event(
    FlippPomodoroApp* app,
    FlippPomodoroAppCustomEvent custom_event) {
    if(custom_event == FlippPomodoroAppCustomEventResumeTimer) {
        scene_manager_next_scene(app->scene_manager, FlippPomodoroSceneTimer);
    }
}

bool flipp_pomodoro_scene_info_on_event(void* ctx, SceneManagerEvent event) {
    furi_assert(ctx);
    FlippPomodoroApp* app = ctx;

    switch(event.type) {
    case SceneManagerEventTypeBack:
        view_dispatcher_stop(app->view_dispatcher);
        return SceneEventConusmed;
    case SceneManagerEventTypeCustom:
        flipp_pomodoro_scene_info_handle_custom_event(app, event.event);
        return SceneEventConusmed;
    default:
        break;
    };
    return SceneEventNotConusmed;
}

void flipp_pomodoro_scene_info_on_exit(void* ctx) {
    UNUSED(ctx);
}