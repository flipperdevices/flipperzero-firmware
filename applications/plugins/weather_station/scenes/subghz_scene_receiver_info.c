#include "../weather_station_app_i.h"
#include "../views/weather_station_receiver.h"

void weather_station_scene_receiver_info_callback(WSCustomEvent event, void* context) {
    furi_assert(context);
    WeatherStationApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

static void weather_station_scene_receiver_info_add_to_history_callback(
    SubGhzReceiver* receiver,
    SubGhzProtocolDecoderBase* decoder_base,
    void* context) {
    furi_assert(context);
    WeatherStationApp* app = context;
    FuriString* str_buff;
    str_buff = furi_string_alloc();

    ws_history_add_to_history(app->txrx->history, decoder_base, app->txrx->preset);
    ws_view_receiver_info_update(
        app->ws_receiver_info,
        ws_history_get_raw_data(app->txrx->history, app->txrx->idx_menu_chosen));
    subghz_receiver_reset(receiver);
    furi_string_free(str_buff);
    app->txrx->rx_key_state = WSRxKeyStateAddKey;
}

void weather_station_scene_receiver_info_on_enter(void* context) {
    WeatherStationApp* app = context;

    subghz_receiver_set_rx_callback(
        app->txrx->receiver, weather_station_scene_receiver_info_add_to_history_callback, app);
    ws_view_receiver_info_update(
        app->ws_receiver_info,
        ws_history_get_raw_data(app->txrx->history, app->txrx->idx_menu_chosen));
    view_dispatcher_switch_to_view(app->view_dispatcher, WeatherStationViewReceiverInfo);
}

bool weather_station_scene_receiver_info_on_event(void* context, SceneManagerEvent event) {
    WeatherStationApp* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);
    return consumed;
}

void weather_station_scene_receiver_info_on_exit(void* context) {
    UNUSED(context);
}
