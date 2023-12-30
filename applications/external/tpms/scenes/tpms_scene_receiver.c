#include "../tpms_app_i.h"
#include "../views/tpms_receiver.h"

static const NotificationSequence subghz_sequence_rx = {
    &message_green_255,

    &message_vibro_on,
    &message_note_c6,
    &message_delay_50,
    &message_sound_off,
    &message_vibro_off,

    &message_delay_50,
    NULL,
};

static const NotificationSequence subghz_sequence_rx_locked = {
    &message_green_255,

    &message_display_backlight_on,

    &message_vibro_on,
    &message_note_c6,
    &message_delay_50,
    &message_sound_off,
    &message_vibro_off,

    &message_delay_500,

    &message_display_backlight_off,
    NULL,
};

static void tpms_scene_receiver_update_statusbar(void* context) {
    TPMSApp* app = context;
    FuriString* history_stat_str;
    history_stat_str = furi_string_alloc();
    if(!tpms_history_get_text_space_left(app->txrx->history, history_stat_str)) {
        FuriString* frequency_str;
        FuriString* modulation_str;

        frequency_str = furi_string_alloc();
        modulation_str = furi_string_alloc();

        tpms_get_frequency_modulation(app, frequency_str, modulation_str);

        tpms_view_receiver_add_data_statusbar(
            app->tpms_receiver,
            furi_string_get_cstr(frequency_str),
            furi_string_get_cstr(modulation_str),
            furi_string_get_cstr(history_stat_str),
            radio_device_loader_is_external(app->txrx->radio_device));

        furi_string_free(frequency_str);
        furi_string_free(modulation_str);
    } else {
        tpms_view_receiver_add_data_statusbar(
            app->tpms_receiver,
            furi_string_get_cstr(history_stat_str),
            "",
            "",
            radio_device_loader_is_external(app->txrx->radio_device));
    }
    furi_string_free(history_stat_str);
}

void tpms_scene_receiver_callback(TPMSCustomEvent event, void* context) {
    furi_assert(context);
    TPMSApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

static void tpms_scene_receiver_add_to_history_callback(
    SubGhzReceiver* receiver,
    SubGhzProtocolDecoderBase* decoder_base,
    void* context) {
    furi_assert(context);
    TPMSApp* app = context;
    FuriString* str_buff;
    str_buff = furi_string_alloc();

    if(tpms_history_add_to_history(app->txrx->history, decoder_base, app->txrx->preset) ==
       TPMSHistoryStateAddKeyNewDada) {
        furi_string_reset(str_buff);

        tpms_history_get_text_item_menu(
            app->txrx->history, str_buff, tpms_history_get_item(app->txrx->history) - 1);
        tpms_view_receiver_add_item_to_menu(
            app->tpms_receiver,
            furi_string_get_cstr(str_buff),
            tpms_history_get_type_protocol(
                app->txrx->history, tpms_history_get_item(app->txrx->history) - 1));

        tpms_scene_receiver_update_statusbar(app);
        notification_message(app->notifications, &sequence_blink_green_10);
        if(app->lock != TPMSLockOn) {
            notification_message(app->notifications, &subghz_sequence_rx);
        } else {
            notification_message(app->notifications, &subghz_sequence_rx_locked);
        }
    }
    subghz_receiver_reset(receiver);
    furi_string_free(str_buff);
    app->txrx->rx_key_state = TPMSRxKeyStateAddKey;
}

void tpms_scene_receiver_on_enter(void* context) {
    TPMSApp* app = context;

    FuriString* str_buff;
    str_buff = furi_string_alloc();

    if(app->txrx->rx_key_state == TPMSRxKeyStateIDLE) {
        tpms_preset_init(
            app, "AM650", subghz_setting_get_default_frequency(app->setting), NULL, 0);
        tpms_history_reset(app->txrx->history);
        app->txrx->rx_key_state = TPMSRxKeyStateStart;
    }

    tpms_view_receiver_set_lock(app->tpms_receiver, app->lock);

    //Load history to receiver
    tpms_view_receiver_exit(app->tpms_receiver);
    for(uint8_t i = 0; i < tpms_history_get_item(app->txrx->history); i++) {
        furi_string_reset(str_buff);
        tpms_history_get_text_item_menu(app->txrx->history, str_buff, i);
        tpms_view_receiver_add_item_to_menu(
            app->tpms_receiver,
            furi_string_get_cstr(str_buff),
            tpms_history_get_type_protocol(app->txrx->history, i));
        app->txrx->rx_key_state = TPMSRxKeyStateAddKey;
    }
    furi_string_free(str_buff);
    tpms_scene_receiver_update_statusbar(app);

    tpms_view_receiver_set_callback(app->tpms_receiver, tpms_scene_receiver_callback, app);
    subghz_receiver_set_rx_callback(
        app->txrx->receiver, tpms_scene_receiver_add_to_history_callback, app);

    if(app->txrx->txrx_state == TPMSTxRxStateRx) {
        tpms_rx_end(app);
    };
    if((app->txrx->txrx_state == TPMSTxRxStateIDLE) ||
       (app->txrx->txrx_state == TPMSTxRxStateSleep)) {
        tpms_begin(
            app,
            subghz_setting_get_preset_data_by_name(
                app->setting, furi_string_get_cstr(app->txrx->preset->name)));

        tpms_rx(app, app->txrx->preset->frequency);
    }

    tpms_view_receiver_set_idx_menu(app->tpms_receiver, app->txrx->idx_menu_chosen);
    view_dispatcher_switch_to_view(app->view_dispatcher, TPMSViewReceiver);
}

bool tpms_scene_receiver_on_event(void* context, SceneManagerEvent event) {
    TPMSApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case TPMSCustomEventViewReceiverBack:
            // Stop CC1101 Rx
            if(app->txrx->txrx_state == TPMSTxRxStateRx) {
                tpms_rx_end(app);
                tpms_sleep(app);
            };
            app->txrx->hopper_state = TPMSHopperStateOFF;
            app->txrx->idx_menu_chosen = 0;
            subghz_receiver_set_rx_callback(app->txrx->receiver, NULL, app);

            app->txrx->rx_key_state = TPMSRxKeyStateIDLE;
            tpms_preset_init(
                app, "AM650", subghz_setting_get_default_frequency(app->setting), NULL, 0);
            if(scene_manager_has_previous_scene(app->scene_manager, TPMSSceneStart)) {
                consumed = scene_manager_search_and_switch_to_previous_scene(
                    app->scene_manager, TPMSSceneStart);
            } else {
                scene_manager_next_scene(app->scene_manager, TPMSSceneStart);
            }
            break;
        case TPMSCustomEventViewReceiverOK:
            app->txrx->idx_menu_chosen = tpms_view_receiver_get_idx_menu(app->tpms_receiver);
            scene_manager_next_scene(app->scene_manager, TPMSSceneReceiverInfo);
            consumed = true;
            break;
        case TPMSCustomEventViewReceiverConfig:
            app->txrx->idx_menu_chosen = tpms_view_receiver_get_idx_menu(app->tpms_receiver);
            scene_manager_next_scene(app->scene_manager, TPMSSceneReceiverConfig);
            consumed = true;
            break;
        case TPMSCustomEventViewReceiverOffDisplay:
            notification_message(app->notifications, &sequence_display_backlight_off);
            consumed = true;
            break;
        case TPMSCustomEventViewReceiverUnlock:
            app->lock = TPMSLockOff;
            consumed = true;
            break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        if(app->txrx->hopper_state != TPMSHopperStateOFF) {
            tpms_hopper_update(app);
            tpms_scene_receiver_update_statusbar(app);
        }
        // Get current RSSI
        float rssi = furi_hal_subghz_get_rssi();
        tpms_view_receiver_set_rssi(app->tpms_receiver, rssi);

        if(app->txrx->txrx_state == TPMSTxRxStateRx) {
            notification_message(app->notifications, &sequence_blink_cyan_10);
        }
    }
    return consumed;
}

void tpms_scene_receiver_on_exit(void* context) {
    UNUSED(context);
}
