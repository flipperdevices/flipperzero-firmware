#include "../subghz_i.h"
#include "../views/subghz_receiver.h"

void subghz_scene_receiver_callback(SubghzReceverEvent event, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    view_dispatcher_send_custom_event(subghz->view_dispatcher, event);
}

void subghz_scene_add_to_history_callback(SubGhzProtocolCommon* parser, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    SubghzReceiver* subghz_receiver = subghz->subghz_receiver;
    string_t str_buff;
    string_init(str_buff);

    if(subghz_history_add_to_history(
           subghz->txrx->history, parser, subghz->txrx->frequency, subghz->txrx->preset)) {
        subghz_protocol_reset(subghz->txrx->protocol);
        string_clean(str_buff);
        subghz_history_get_text_item_menu(
            subghz->txrx->history, str_buff, subghz_history_get_item(subghz->txrx->history) - 1);
        subghz_receiver_add_item_to_menu(
            subghz_receiver,
            string_get_cstr(str_buff),
            subghz_history_get_type_protocol(
                subghz->txrx->history, subghz_history_get_item(subghz->txrx->history) - 1));
    }
    string_clear(str_buff);
}

const void subghz_scene_receiver_on_enter(void* context) {
    SubGhz* subghz = context;
    SubghzReceiver* subghz_receiver = subghz->subghz_receiver;
    string_t str_buff;
    string_init(str_buff);

    for(uint8_t i = 0; i < subghz_history_get_item(subghz->txrx->history); i++) {
        string_clean(str_buff);
        subghz_history_get_text_item_menu(subghz->txrx->history, str_buff, i);
        subghz_receiver_add_item_to_menu(
            subghz_receiver,
            string_get_cstr(str_buff),
            subghz_history_get_type_protocol(subghz->txrx->history, i));
    }
    string_clear(str_buff);
    subghz_receiver_set_callback(subghz_receiver, subghz_scene_receiver_callback, subghz);
    subghz_protocol_enable_dump(
        subghz->txrx->protocol, subghz_scene_add_to_history_callback, subghz);

    //subghz_receiver_set_protocol(subghz_receiver, subghz->protocol_result, subghz->protocol);
    //subghz_receiver_set_worker(subghz_receiver, subghz->worker);

    subghz->state_notifications = NOTIFICATION_RX_STATE;
    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewReceiver);
}

const bool subghz_scene_receiver_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        // case SubghzReceverEventSave:
        //     subghz->state_notifications = NOTIFICATION_IDLE_STATE;
        //     // subghz->frequency = subghz_receiver_get_frequency(subghz->subghz_receiver);
        //     // subghz->preset = subghz_receiver_get_preset(subghz->subghz_receiver);
        //     // subghz->protocol_result = subghz_receiver_get_protocol(subghz->subghz_receiver);
        //     scene_manager_next_scene(subghz->scene_manager, SubGhzSceneSaveName);
        //     return true;
        //     break;
        case SubghzReceverEventBack:
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneStart);
            return true;
            break;
        case SubghzReceverEventOK:
            subghz->txrx->idx_menu_chosen =
                subghz_receiver_get_idx_menu_ok(subghz->subghz_receiver);
            //scene_manager_next_scene(subghz->scene_manager, SubGhzSceneReceiverInfo);
            return true;
            break;
        // case SubghzReceverEventSendStart:
        //     // subghz->state_notifications = NOTIFICATION_TX_STATE;
        //     // subghz->frequency = subghz_receiver_get_frequency(subghz->subghz_receiver);
        //     // subghz->preset = subghz_receiver_get_preset(subghz->subghz_receiver);
        //     // subghz->protocol_result = subghz_receiver_get_protocol(subghz->subghz_receiver);
        //     // subghz_transmitter_tx_start(subghz);
        //     return true;
        //     break;
        // case SubghzReceverEventSendStop:
        //     subghz->state_notifications = NOTIFICATION_IDLE_STATE;
        //     // subghz_transmitter_tx_stop(subghz);
        //     return true;
        //     break;
        // case SubghzReceverEventMain:
        //     subghz->state_notifications = NOTIFICATION_RX_STATE;
        //     return true;
        //     break;
        case SubghzReceverEventConfig:
            subghz->state_notifications = NOTIFICATION_IDLE_STATE;
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneReceiverConfig);
            return true;
            break;
        // case SubghzReceverEventSendHistoryFull:
        //     subghz->state_notifications = NOTIFICATION_IDLE_STATE;
        //     return true;
        //     break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        switch(subghz->state_notifications) {
        case NOTIFICATION_TX_STATE:
            notification_message(subghz->notifications, &sequence_blink_red_10);
            break;
        case NOTIFICATION_RX_STATE:
            notification_message(subghz->notifications, &sequence_blink_blue_10);
            break;
        default:
            break;
        }
    }
    return false;
}

const void subghz_scene_receiver_on_exit(void* context) {
    SubGhz* subghz = context;
        // Stop CC1101 Rx
    subghz_rx_end(subghz->txrx->worker);
    subghz_sleep();


    subghz_protocol_enable_dump(subghz->txrx->protocol, NULL, subghz);
    subghz_receiver_set_callback(subghz->subghz_receiver, NULL, subghz);
}
