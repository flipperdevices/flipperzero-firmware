#include "ttt_multi.h"

bool ttt_multi_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    TttMultiApp* ttt_multi = context;
    return scene_manager_handle_custom_event(ttt_multi->scene_manager, event);
}

bool ttt_multi_back_event_callback(void* context) {
    furi_assert(context);
    TttMultiApp* ttt_multi = context;
    return scene_manager_handle_back_event(ttt_multi->scene_manager);
}

void ttt_multi_tick_event_callback(void* context) {
    furi_assert(context);
    TttMultiApp* ttt_multi = context;
    scene_manager_handle_tick_event(ttt_multi->scene_manager);
}

TttMultiApp* ttt_multi_alloc() {
    TttMultiApp* ttt_multi = malloc(sizeof(TttMultiApp));

    ttt_multi->view_dispatcher = view_dispatcher_alloc();
    ttt_multi->scene_manager = scene_manager_alloc(&ttt_multi_scene_handlers, ttt_multi);

    view_dispatcher_enable_queue(ttt_multi->view_dispatcher);
    view_dispatcher_set_event_callback_context(ttt_multi->view_dispatcher, ttt_multi);
    view_dispatcher_set_custom_event_callback(
        ttt_multi->view_dispatcher, ttt_multi_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        ttt_multi->view_dispatcher, ttt_multi_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        ttt_multi->view_dispatcher, ttt_multi_tick_event_callback, 100);

    ttt_multi->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        ttt_multi->view_dispatcher, ttt_multi->gui, ViewDispatcherTypeFullscreen);

    ttt_multi->notifications = furi_record_open(RECORD_NOTIFICATION);

    ttt_multi->game_view = ttt_multi_game_view_alloc();
    view_dispatcher_add_view(
        ttt_multi->view_dispatcher,
        TttMultiViewGame,
        ttt_multi_game_get_view(ttt_multi->game_view));

    ttt_multi->submenu = submenu_alloc();
    view_dispatcher_add_view(
        ttt_multi->view_dispatcher, TttMultiViewMenu, submenu_get_view(ttt_multi->submenu));

    return ttt_multi;
}

void ttt_multi_free(TttMultiApp* ttt_multi) {
    furi_assert(ttt_multi);

    notification_message(ttt_multi->notifications, &sequence_blink_stop);
    furi_record_close(RECORD_NOTIFICATION);
    ttt_multi->notifications = NULL;

    view_dispatcher_remove_view(ttt_multi->view_dispatcher, TttMultiViewGame);
    ttt_multi_game_view_free(ttt_multi->game_view);

    view_dispatcher_remove_view(ttt_multi->view_dispatcher, TttMultiViewMenu);
    submenu_free(ttt_multi->submenu);

    scene_manager_free(ttt_multi->scene_manager);
    view_dispatcher_free(ttt_multi->view_dispatcher);

    furi_record_close(RECORD_GUI);
    ttt_multi->gui = NULL;

    free(ttt_multi);
}

int32_t ttt_multi_app(void* p) {
    UNUSED(p);
    TttMultiApp* ttt_multi = ttt_multi_alloc();

    scene_manager_next_scene(ttt_multi->scene_manager, TttMultiSceneStart);
    view_dispatcher_run(ttt_multi->view_dispatcher);

    ttt_multi_free(ttt_multi);

    return 0;
}