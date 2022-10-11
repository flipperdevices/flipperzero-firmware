#include "../weather_station_app_i.h"

static void weather_station_scene_show_callback(
    SubGhzReceiver* receiver,
    SubGhzProtocolDecoderBase* decoder_base,
    void* context) {
    furi_assert(context);
    WeatherStationApp* app = context;
    UNUSED(app);
    FuriString* str_buff;
    str_buff = furi_string_alloc();

    subghz_protocol_decoder_base_get_string(decoder_base, str_buff);
    weather_station_show_add_data_to_show(app->weather_station_show, furi_string_get_cstr(str_buff));

    subghz_receiver_reset(receiver);
    furi_string_free(str_buff);
}

void weather_station_scene_show_on_enter(void* context) {
    WeatherStationApp* app = context;

    view_dispatcher_switch_to_view(app->view_dispatcher, WeatherStationViewShow);

    FuriString* key_str;
    key_str = furi_string_alloc();

    furi_string_free(key_str);

    if(app->txrx->txrx_state == WSTxRxStateRx) {
        ws_rx_end(app);
    };
    if((app->txrx->txrx_state == WSTxRxStateIDLE) || (app->txrx->txrx_state == WSTxRxStateSleep)) {
        ws_begin(app, NULL);
        ws_rx(app, app->txrx->preset->frequency);
    }

    subghz_receiver_set_filter(app->txrx->receiver, SubGhzProtocolFlag_Decodable);
    subghz_receiver_set_rx_callback(app->txrx->receiver, weather_station_scene_show_callback, app);

    //tx(app);
}

bool weather_station_scene_show_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    //WeatherStationApp* app = context;
    bool consumed = false;
    // if(event.type == SceneManagerEventTypeCustom) {
    //     if(event.event == WeatherStationPwmEventUpdate) {
    //         consumed = true;
    //         furi_hal_pwm_set_params(app->pwm_ch, app->pwm_freq, app->pwm_duty);
    //     } else if(event.event == WeatherStationPwmEventChannelChange) {
    //         consumed = true;
    //         furi_hal_pwm_stop(app->pwm_ch_prev);
    //         furi_hal_pwm_start(app->pwm_ch, app->pwm_freq, app->pwm_duty);
    //     }
    // }
    return consumed;
}

void weather_station_scene_show_on_exit(void* context) {
    WeatherStationApp* app = context;
    if(app->txrx->txrx_state == WSTxRxStateRx) {
        ws_rx_end(app);
    };
    //variable_item_list_reset(app->var_item_list);
    //furi_hal_pwm_stop(app->pwm_ch);
}
