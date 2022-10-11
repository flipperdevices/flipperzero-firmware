#include "../weather_station_app_i.h"
#include "../views/weather_station_receiver.h"

static void weather_station_scene_receiver_update_statusbar(void* context) {
    WeatherStationApp* app = context;
    string_t history_stat_str;
    string_init(history_stat_str);
    if(!ws_history_get_text_space_left(app->txrx->history, history_stat_str)) {
        string_t frequency_str;
        string_t modulation_str;

        string_init(frequency_str);
        string_init(modulation_str);

        ws_get_frequency_modulation(app, frequency_str, modulation_str);

        ws_view_receiver_add_data_statusbar(
            app->ws_receiver,
            string_get_cstr(frequency_str),
            string_get_cstr(modulation_str),
            string_get_cstr(history_stat_str));

        string_clear(frequency_str);
        string_clear(modulation_str);
    } else {
        ws_view_receiver_add_data_statusbar(
            app->ws_receiver, string_get_cstr(history_stat_str), "", "");
    }
    string_clear(history_stat_str);
}

void weather_station_scene_receiver_callback(WSCustomEvent event, void* context) {
    furi_assert(context);
    WeatherStationApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

static void weather_station_scene_add_to_history_callback(
    SubGhzReceiver* receiver,
    SubGhzProtocolDecoderBase* decoder_base,
    void* context) {
    furi_assert(context);
    WeatherStationApp* app = context;
    string_t str_buff;
    string_init(str_buff);

    if(ws_history_add_to_history(app->txrx->history, decoder_base, app->txrx->preset)) {
        string_reset(str_buff);

        ws_history_get_text_item_menu(
            app->txrx->history, str_buff, ws_history_get_item(app->txrx->history) - 1);
        ws_view_receiver_add_item_to_menu(
            app->ws_receiver,
            string_get_cstr(str_buff),
            ws_history_get_type_protocol(
                app->txrx->history, ws_history_get_item(app->txrx->history) - 1));

        weather_station_scene_receiver_update_statusbar(app);
    }
    subghz_receiver_reset(receiver);
    string_clear(str_buff);
    app->txrx->rx_key_state = WSRxKeyStateAddKey;
}

void weather_station_scene_receiver_on_enter(void* context) {
    WeatherStationApp* app = context;

    string_t str_buff;
    string_init(str_buff);

    if(app->txrx->rx_key_state == WSRxKeyStateIDLE) {
        ws_preset_init(
        app, "AM650", subghz_setting_get_default_frequency(app->setting), NULL, 0);
        ws_history_reset(app->txrx->history);
        app->txrx->rx_key_state = WSRxKeyStateStart;
    }

    ws_view_receiver_set_lock(app->ws_receiver, app->lock);

    //Load history to receiver
    ws_view_receiver_exit(app->ws_receiver);
    for(uint8_t i = 0; i < ws_history_get_item(app->txrx->history); i++) {
        string_reset(str_buff);
        ws_history_get_text_item_menu(app->txrx->history, str_buff, i);
        ws_view_receiver_add_item_to_menu(
            app->ws_receiver,
            string_get_cstr(str_buff),
            ws_history_get_type_protocol(app->txrx->history, i));
       app->txrx->rx_key_state = WSRxKeyStateAddKey;
    }
    string_clear(str_buff);
    weather_station_scene_receiver_update_statusbar(app);

    ws_view_receiver_set_callback(
        app->ws_receiver, weather_station_scene_receiver_callback, app);
    subghz_receiver_set_rx_callback(
        app->txrx->receiver, weather_station_scene_add_to_history_callback, app);


    //app->state_notifications = SubGhzNotificationStateRx;
    if(app->txrx->txrx_state == WSTxRxStateRx) {
        ws_rx_end(app);
    };
    if((app->txrx->txrx_state == WSTxRxStateIDLE) || (app->txrx->txrx_state == WSTxRxStateSleep)) {
        ws_begin(
            app,
            subghz_setting_get_preset_data_by_name(
                app->setting, string_get_cstr(app->txrx->preset->name)));

        ws_rx(app, app->txrx->preset->frequency);
    }

    ws_view_receiver_set_idx_menu(app->ws_receiver, app->txrx->idx_menu_chosen);
    view_dispatcher_switch_to_view(app->view_dispatcher, WeatherStationViewReceiver);
}

bool weather_station_scene_receiver_on_event(void* context, SceneManagerEvent event) {
    WeatherStationApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case WSCustomEventViewReceiverBack:
            // Stop CC1101 Rx
            //app->state_notifications = SubGhzNotificationStateIDLE;
            if(app->txrx->txrx_state == WSTxRxStateRx) {
                ws_rx_end(app);
                ws_sleep(app);
            };
            //app->txrx->hopper_state = SubGhzHopperStateOFF;
            app->txrx->idx_menu_chosen = 0;
            subghz_receiver_set_rx_callback(app->txrx->receiver, NULL, app);

            // if(subghz->txrx->rx_key_state == WSRxKeyStateAddKey) {
            //     subghz->txrx->rx_key_state = WSRxKeyStateExit;
            //     scene_manager_next_scene(subghz->scene_manager, WeatherStationSceneNeedSaving);
            // } else {
            app->txrx->rx_key_state = WSRxKeyStateIDLE;
            ws_preset_init(
                    app,
                    "AM650",
                    subghz_setting_get_default_frequency(app->setting),
                    NULL,
                    0);
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, WeatherStationSceneStart);
            //}
            consumed = true;
            break;
        // case WSCustomEventViewReceiverOK:
        //     subghz->txrx->idx_menu_chosen =
        //         ws_view_receiver_get_idx_menu(subghz->ws_receiver);
        //     scene_manager_next_scene(subghz->scene_manager, WeatherStationSceneReceiverInfo);
        //     consumed = true;
        //     break;
        case WSCustomEventViewReceiverConfig:
            //subghz->state_notifications = SubGhzNotificationStateIDLE;
            app->txrx->idx_menu_chosen =
                ws_view_receiver_get_idx_menu(app->ws_receiver);
            scene_manager_next_scene(app->scene_manager, WeatherStationSceneReceiverConfig);
            consumed = true;
            break;
        // case WSCustomEventViewReceiverOffDisplay:
        //     notification_message(app->notifications, &sequence_display_backlight_off);
        //     consumed = true;
        //     break;
        case WSCustomEventViewReceiverUnlock:
            app->lock = WSLockOff;
            consumed = true;
            break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        //if(app->txrx->hopper_state != SubGhzHopperStateOFF) {
        //subghz_hopper_update(subghz);
        weather_station_scene_receiver_update_statusbar(app);
        // }
        // switch(subghz->state_notifications) {
        // case SubGhzNotificationStateRx:
        //     notification_message(subghz->notifications, &sequence_blink_cyan_10);
        //     break;
        // case SubGhzNotificationStateRxDone:
        //     if(subghz->lock != WSLockOn) {
        //         notification_message(subghz->notifications, &subghs_sequence_rx);
        //     } else {
        //         notification_message(subghz->notifications, &subghs_sequence_rx_locked);
        //     }
        //     subghz->state_notifications = SubGhzNotificationStateRx;
        //     break;
        // default:
        //     break;
        // }
    }
    return consumed;
}

void weather_station_scene_receiver_on_exit(void* context) {
    UNUSED(context);
}
