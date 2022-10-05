#include "../weather_station_app_i.h"

void get_string(string_t output) {
    string_cat_printf(
        output,
        "%s %dbit\r\n"
        "Key:0x%08lX\r\n"
        "Ch:%d \r\n"
        "T:%d.%dC  H:%d%\r\n",
        "TermoPRO",
        24,
        0x1122AA,
        1,
        21,
        13,
        52);
}

void weather_station_scene_show_on_enter(void* context) {
    WeatherStationApp* app = context;

    view_dispatcher_switch_to_view(app->view_dispatcher, WeatherStationViewShow);

    string_t key_str;
    string_init(key_str);
    get_string(key_str);
    weather_station_show_add_data_to_show(app->weather_station_show, string_get_cstr(key_str));

    string_clear(key_str);

    if(app->txrx->txrx_state == WeatherStationTxRxStateRx) {
        weather_station_rx_end(app);
    };
    if((app->txrx->txrx_state == WeatherStationTxRxStateIDLE) ||
       (app->txrx->txrx_state == WeatherStationTxRxStateSleep)) {
        weather_station_begin(app, app->txrx->preset->preset);
        weather_station_rx(app, app->txrx->preset->frequency);
    }
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
    if(app->txrx->txrx_state == WeatherStationTxRxStateRx) {
        weather_station_rx_end(app);
    };
    //variable_item_list_reset(app->var_item_list);
    //furi_hal_pwm_stop(app->pwm_ch);
}
