#include "../subghz_i.h"
#include "../views/subghz_save_raw.h"
#include <lib/subghz/protocols/subghz_protocol_raw.h>

static void subghz_scene_save_raw_update_statusbar(void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    char frequency_str[20];
    char preset_str[10];

    snprintf(
        frequency_str,
        sizeof(frequency_str),
        "%03ld.%02ld",
        subghz->txrx->frequency / 1000000 % 1000,
        subghz->txrx->frequency / 10000 % 100);
    if(subghz->txrx->preset == FuriHalSubGhzPresetOok650Async ||
       subghz->txrx->preset == FuriHalSubGhzPresetOok270Async) {
        snprintf(preset_str, sizeof(preset_str), "AM");
    } else if(
        subghz->txrx->preset == FuriHalSubGhzPreset2FSKDev238Async ||
        subghz->txrx->preset == FuriHalSubGhzPreset2FSKDev476Async) {
        snprintf(preset_str, sizeof(preset_str), "FM");
    } else {
        furi_crash(NULL);
    }
    subghz_save_raw_add_data_statusbar(subghz->subghz_save_raw, frequency_str, preset_str);
}

void subghz_scene_save_raw_parse(SubGhzProtocolCommon* instance, bool level, uint32_t duration) {
    subghz_protocol_raw_parse((SubGhzProtocolRAW*)instance, level, duration);
}

void subghz_scene_save_raw_callback(SubghzSaveRAWEvent event, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    view_dispatcher_send_custom_event(subghz->view_dispatcher, event);
}

void subghz_scene_save_raw_on_enter(void* context) {
    SubGhz* subghz = context;
    subghz_scene_save_raw_update_statusbar(subghz);
    subghz_save_raw_set_callback(subghz->subghz_save_raw, subghz_scene_save_raw_callback, subghz);

    subghz->txrx->protocol_save_raw = (SubGhzProtocolCommon*)subghz_protocol_raw_alloc();
    subghz_protocol_raw_reset((SubGhzProtocolRAW*)subghz->txrx->protocol_save_raw);

    subghz_worker_set_overrun_callback(subghz->txrx->worker, NULL);
    subghz_worker_set_pair_callback(
        subghz->txrx->worker, (SubGhzWorkerPairCallback)subghz_scene_save_raw_parse);
    subghz_worker_set_context(subghz->txrx->worker, subghz->txrx->protocol_save_raw);

    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewSaveRAW);
}

bool subghz_scene_save_raw_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case SubghzSaveRAWEventBack:
            if(subghz->txrx->txrx_state == SubGhzTxRxStateRx) {
                subghz_rx_end(subghz);
                subghz_sleep(subghz);
            };
            subghz->txrx->frequency = subghz_frequencies[subghz_frequencies_433_92];
            subghz->txrx->preset = FuriHalSubGhzPresetOok650Async;
            subghz_protocol_save_raw_to_file_stop(
                (SubGhzProtocolRAW*)subghz->txrx->protocol_save_raw);
            scene_manager_search_and_switch_to_previous_scene(
                subghz->scene_manager, SubGhzSceneStart);
            return true;
            break;
        case SubghzSaveRAWEventConfig:
            scene_manager_set_scene_state(subghz->scene_manager, SubGhzSceneSaveRAW, 1);
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneReceiverConfig);
            return true;
            break;
        case SubghzSaveRAWEventIDLE:
            if(subghz->txrx->txrx_state == SubGhzTxRxStateRx) {
                subghz_rx_end(subghz);
                subghz_sleep(subghz);
            };
            subghz_protocol_save_raw_to_file_stop(
                (SubGhzProtocolRAW*)subghz->txrx->protocol_save_raw);
            subghz->state_notifications = NOTIFICATION_IDLE_STATE;
            return true;
            break;
        case SubghzSaveRAWEventREC:
            if(subghz_protocol_save_raw_to_file_init(
                   (SubGhzProtocolRAW*)subghz->txrx->protocol_save_raw,
                   "Raw",
                   subghz->txrx->frequency,
                   subghz->txrx->preset)) {
                if((subghz->txrx->txrx_state == SubGhzTxRxStateIdle) ||
                   (subghz->txrx->txrx_state == SubGhzTxRxStateSleep)) {
                    subghz_begin(subghz, subghz->txrx->preset);
                    subghz_rx(subghz, subghz->txrx->frequency);
                }
                subghz->state_notifications = NOTIFICATION_RX_STATE;
            } else {
                string_set(subghz->error_str, "No SD card");
                scene_manager_next_scene(subghz->scene_manager, SubGhzSceneShowError);
            }
            return true;
            break;
        case SubghzSaveRAWEventMore:
            //scene_manager_next_scene(subghz->scene_manager, SubGhzSceneReceiverConfig);
            return true;
            break;

        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        switch(subghz->state_notifications) {
        case NOTIFICATION_RX_STATE:
            notification_message(subghz->notifications, &sequence_blink_blue_10);
            subghz_save_raw_update_sample_write(
                subghz->subghz_save_raw,
                subghz_save_protocol_raw_get_sample_write(
                    (SubGhzProtocolRAW*)subghz->txrx->protocol_save_raw));
            subghz_save_raw_add_data_rssi(subghz->subghz_save_raw, furi_hal_subghz_get_rssi());
            break;
        default:
            break;
        }
    }
    return false;
}

void subghz_scene_save_raw_on_exit(void* context) {
    SubGhz* subghz = context;

    //Stop CC1101
    if(subghz->txrx->txrx_state == SubGhzTxRxStateRx) {
        subghz_rx_end(subghz);
        subghz_sleep(subghz);
    };
    subghz->state_notifications = NOTIFICATION_IDLE_STATE;

    //Сallback restoration
    subghz_worker_set_overrun_callback(
        subghz->txrx->worker, (SubGhzWorkerOverrunCallback)subghz_parser_reset);
    subghz_worker_set_pair_callback(
        subghz->txrx->worker, (SubGhzWorkerPairCallback)subghz_parser_parse);
    subghz_worker_set_context(subghz->txrx->worker, subghz->txrx->parser);

    //Free RAW protocol
    subghz_protocol_raw_free((SubGhzProtocolRAW*)subghz->txrx->protocol_save_raw);
}
