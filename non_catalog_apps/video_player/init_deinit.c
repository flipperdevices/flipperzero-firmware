#include "init_deinit.h"

PlayerView* player_view_alloc(VideoPlayerApp* player) {
    PlayerView* player_view = malloc(sizeof(PlayerView));
    player_view->view = view_alloc();
    player_view->context = player;
    view_set_context(player_view->view, player_view);
    view_allocate_model(player_view->view, ViewModelTypeLocking, sizeof(PlayerViewModel));
    view_set_draw_callback(player_view->view, draw_callback);
    view_set_input_callback(player_view->view, input_callback);

    return player_view;
}

VideoPlayerApp* init_player() {
    VideoPlayerApp* player = malloc(sizeof(VideoPlayerApp));
    memset(player, 0, sizeof(VideoPlayerApp));

    // Очередь событий на 8 элементов размера VideoPlayerEvent
    player->event_queue = furi_message_queue_alloc(8, sizeof(VideoPlayerEvent));

    player->gui = furi_record_open(RECORD_GUI);
    player->view_dispatcher = view_dispatcher_alloc();

    player->player_view = player_view_alloc(player);

    view_dispatcher_add_view(player->view_dispatcher, VIEW_PLAYER, player->player_view->view);
    view_dispatcher_attach_to_gui(
        player->view_dispatcher, player->gui, ViewDispatcherTypeFullscreen);

    with_view_model(
        player->player_view->view, PlayerViewModel * model, { model->player = player; }, true);

    player->storage = furi_record_open(RECORD_STORAGE);
    player->stream = file_stream_alloc(player->storage);

    player->notification = furi_record_open(RECORD_NOTIFICATION);
    notification_message(player->notification, &sequence_display_backlight_enforce_on);

    return player;
}

void deinit_player(VideoPlayerApp* player) {
    notification_message(player->notification, &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);

    // Специальная очистка памяти, занимаемой очередью
    furi_message_queue_free(player->event_queue);

    /*view_dispatcher_remove_view(player->view_dispatcher, VIEW_PLAYER);

    view_dispatcher_free(player->view_dispatcher);

    player_view_free(player->player_view);
    furi_record_close(RECORD_GUI);*/

    stream_free(player->stream);
    furi_record_close(RECORD_STORAGE);

    if(player->buffer) {
        free(player->buffer);
    }

    if(player->fake_audio_buffer) {
        free(player->fake_audio_buffer);
    }

    furi_pubsub_unsubscribe(player->input, player->input_subscription);

    player->canvas = NULL;
    gui_direct_draw_release(player->gui);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_INPUT_EVENTS);

    free(player);
}