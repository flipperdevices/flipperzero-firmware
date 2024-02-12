#include "../subghz_i.h"
#include <dolphin/dolphin.h>
#include <lib/subghz/protocols/bin_raw.h>
#include <toolbox/name_generator.h>

#define TAG "SubGhzSceneReceiver"

const NotificationSequence subghz_sequence_rx = {
    &message_green_255,

    &message_display_backlight_on,

    &message_vibro_on,
    &message_note_c6,
    &message_delay_50,
    &message_sound_off,
    &message_vibro_off,

    &message_delay_50,
    NULL,
};

const NotificationSequence subghz_sequence_rx_locked = {
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

const NotificationSequence subghz_sequence_tx_beep = {
    &message_vibro_on,
    &message_note_c6,
    &message_delay_50,
    &message_sound_off,
    &message_vibro_off,
    &message_delay_50,
    NULL,
};

static void subghz_scene_receiver_update_statusbar(void* context) {
    SubGhz* subghz = context;
    FuriString* history_stat_str = furi_string_alloc();
    if(!subghz_history_get_text_space_left(
           subghz->history,
           history_stat_str,
           subghz->gps->satellites,
           subghz->last_settings->delete_old_signals)) {
        FuriString* frequency_str = furi_string_alloc();
        FuriString* modulation_str = furi_string_alloc();

#ifdef SUBGHZ_EXT_PRESET_NAME
        if(subghz_history_get_last_index(subghz->history) > 0) {
            subghz_txrx_get_frequency_and_modulation(
                subghz->txrx, frequency_str, modulation_str, false);
        } else {
            FuriString* temp_str = furi_string_alloc();

            subghz_txrx_get_frequency_and_modulation(subghz->txrx, frequency_str, temp_str, true);
            furi_string_printf(
                modulation_str,
                "%s        Mod: %s",
                (subghz_txrx_radio_device_get(subghz->txrx) == SubGhzRadioDeviceTypeInternal) ?
                    "Int" :
                    "Ext",
                furi_string_get_cstr(temp_str));
            furi_string_free(temp_str);
        }
#else
        subghz_txrx_get_frequency_and_modulation(
            subghz->txrx, frequency_str, modulation_str, false);
#endif

        subghz_view_receiver_add_data_statusbar(
            subghz->subghz_receiver,
            furi_string_get_cstr(frequency_str),
            furi_string_get_cstr(modulation_str),
            furi_string_get_cstr(history_stat_str),
            subghz_txrx_hopper_get_state(subghz->txrx) != SubGhzHopperStateOFF,
            READ_BIT(subghz->filter, SubGhzProtocolFlag_BinRAW) > 0,
            subghz->repeater);

        furi_string_free(frequency_str);
        furi_string_free(modulation_str);
    } else {
        subghz_view_receiver_add_data_statusbar(
            subghz->subghz_receiver,
            furi_string_get_cstr(history_stat_str),
            "",
            "",
            subghz_txrx_hopper_get_state(subghz->txrx) != SubGhzHopperStateOFF,
            READ_BIT(subghz->filter, SubGhzProtocolFlag_BinRAW) > 0,
            subghz->repeater);
    }
    furi_string_free(history_stat_str);

    subghz_view_receiver_set_radio_device_type(
        subghz->subghz_receiver, subghz_txrx_radio_device_get(subghz->txrx));
}

void subghz_scene_receiver_callback(SubGhzCustomEvent event, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    view_dispatcher_send_custom_event(subghz->view_dispatcher, event);
}

void repeater_stop_callback(void* context) {
    SubGhz* subghz = context;
    scene_manager_handle_custom_event(subghz->scene_manager, SubGhzCustomEventViewRepeaterStop);
}

static void subghz_scene_add_to_history_callback(
    SubGhzReceiver* receiver,
    SubGhzProtocolDecoderBase* decoder_base,
    void* context) {
    furi_assert(context);
    SubGhz* subghz = context;

    // The check can be moved to /lib/subghz/receiver.c, but may result in false positives
    if((decoder_base->protocol->filter & subghz->ignore_filter) == 0) {
        SubGhzHistory* history = subghz->history;
        FuriString* item_name = furi_string_alloc();
        FuriString* item_time = furi_string_alloc();
        uint16_t idx = subghz_history_get_item(history);

        SubGhzRadioPreset preset = subghz_txrx_get_preset(subghz->txrx);
        preset.latitude = subghz->gps->latitude;
        preset.longitude = subghz->gps->longitude;

        if(subghz->last_settings->delete_old_signals && subghz_history_full(subghz->history)) {
            subghz_view_receiver_disable_draw_callback(subghz->subghz_receiver);

            while(idx > 0 && subghz_history_full(subghz->history)) {
                subghz_history_delete_item(subghz->history, 0);
                subghz_view_receiver_delete_item(subghz->subghz_receiver, 0);
                idx--;
            }

            subghz_view_receiver_enable_draw_callback(subghz->subghz_receiver);
            if(idx == 0) {
                subghz_rx_key_state_set(subghz, SubGhzRxKeyStateStart);
            }
            subghz_scene_receiver_update_statusbar(subghz);
            subghz->idx_menu_chosen = subghz_view_receiver_get_idx_menu(subghz->subghz_receiver);
        }

        if(subghz_history_add_to_history(history, decoder_base, &preset)) {
            furi_string_reset(item_name);
            furi_string_reset(item_time);

            //If the repeater is on, dont add to the menu, just TX the signal.
            if(subghz->repeater != SubGhzRepeaterStateOff) {
                view_dispatcher_send_custom_event(
                    subghz->view_dispatcher, SubGhzCustomEventViewRepeaterStart);
            } else {
                subghz->state_notifications = SubGhzNotificationStateRxDone;

                if(subghz->remove_duplicates) {
                    // Look in history for signal hash
                    uint32_t hash_data =
                        subghz_protocol_decoder_base_get_hash_data_long(decoder_base);
                    subghz_view_receiver_disable_draw_callback(subghz->subghz_receiver);
                    for(uint16_t i = idx; i > 0; i--) {
                        i--; // Iterating in reverse with off by one
                        if(subghz_history_get_hash_data(subghz->history, i) == hash_data &&
                           subghz_history_get_protocol(subghz->history, i) ==
                               decoder_base->protocol) {
                            // Remove previous instance and update menu index
                            subghz_history_delete_item(subghz->history, i);
                            subghz_view_receiver_delete_item(subghz->subghz_receiver, i);
                            idx--;
                        }
                        i++;
                    }
                    // Restore ui state
                    subghz->idx_menu_chosen =
                        subghz_view_receiver_get_idx_menu(subghz->subghz_receiver);
                    subghz_view_receiver_enable_draw_callback(subghz->subghz_receiver);
                    if(idx == 0) {
                        subghz_rx_key_state_set(subghz, SubGhzRxKeyStateStart);
                    }
                }

                subghz_history_get_text_item_menu(history, item_name, idx);
                subghz_history_get_time_item_menu(history, item_time, idx);
                subghz_view_receiver_add_item_to_menu(
                    subghz->subghz_receiver,
                    furi_string_get_cstr(item_name),
                    furi_string_get_cstr(item_time),
                    subghz_history_get_type_protocol(history, idx),
                    subghz_history_get_repeats(history, idx));

                if(decoder_base->protocol->flag & SubGhzProtocolFlag_Save &&
                   subghz->last_settings->autosave) {
                    // File name
                    char file[SUBGHZ_MAX_LEN_NAME] = {0};
                    const char* suf = subghz->last_settings->protocol_file_names ?
                                          decoder_base->protocol->name :
                                          SUBGHZ_APP_FILENAME_PREFIX;
                    FuriHalRtcDateTime time = subghz_history_get_datetime(history, idx);
                    name_generator_make_detailed_datetime(file, sizeof(file), suf, &time, true);
                    // Dir name
                    FuriString* path = furi_string_alloc_set(SUBGHZ_APP_FOLDER "/Autosave");
                    char* dir = strdup(furi_string_get_cstr(path));
                    // Find non-existent path
                    const char* ext = SUBGHZ_APP_FILENAME_EXTENSION;
                    Storage* storage = furi_record_open(RECORD_STORAGE);
                    storage_get_next_filename(storage, dir, file, ext, path, sizeof(file));
                    strlcpy(file, furi_string_get_cstr(path), sizeof(file));
                    furi_string_printf(path, "%s/%s%s", dir, file, ext);
                    furi_record_close(RECORD_STORAGE);
                    free(dir);
                    // Save
                    subghz_save_protocol_to_file(
                        subghz,
                        subghz_history_get_raw_data(history, idx),
                        furi_string_get_cstr(path));
                    furi_string_free(path);
                }

                subghz_scene_receiver_update_statusbar(subghz);
                if(!subghz->last_settings->delete_old_signals &&
                   subghz_history_full(subghz->history)) {
                    subghz->state_notifications = SubGhzNotificationStateIDLE;
                    notification_message(subghz->notifications, &sequence_error);
                } else {
                    subghz->state_notifications = SubGhzNotificationStateRxDone;
                }
            }
        }
        subghz_receiver_reset(receiver);
        furi_string_free(item_name);
        furi_string_free(item_time);
        subghz_rx_key_state_set(subghz, SubGhzRxKeyStateAddKey);
    } else {
        FURI_LOG_D(TAG, "%s protocol ignored", decoder_base->protocol->name);
    }
}

void subghz_scene_receiver_on_enter(void* context) {
    SubGhz* subghz = context;
    SubGhzHistory* history = subghz->history;

    FuriString* item_name = furi_string_alloc();
    FuriString* item_time = furi_string_alloc();

    if(subghz_rx_key_state_get(subghz) == SubGhzRxKeyStateIDLE) {
#if SUBGHZ_LAST_SETTING_SAVE_PRESET
        subghz_txrx_set_preset_internal(
            subghz->txrx, subghz->last_settings->frequency, subghz->last_settings->preset_index);
#else
        subghz_txrx_set_default_preset(subghz->txrx, subghz->last_settings->frequency);
#endif

        subghz->filter = subghz->last_settings->filter;
        subghz_txrx_receiver_set_filter(subghz->txrx, subghz->filter);
        subghz->ignore_filter = subghz->last_settings->ignore_filter;

        subghz_history_reset(history);
        subghz_rx_key_state_set(subghz, SubGhzRxKeyStateStart);
        subghz->idx_menu_chosen = 0;
    }

    subghz_view_receiver_set_mode(subghz->subghz_receiver, SubGhzViewReceiverModeLive);

    // Load history to receiver
    subghz_view_receiver_exit(subghz->subghz_receiver);
    for(uint16_t i = 0; i < subghz_history_get_item(history); i++) {
        furi_string_reset(item_name);
        furi_string_reset(item_time);
        subghz_history_get_text_item_menu(history, item_name, i);
        subghz_history_get_time_item_menu(history, item_time, i);
        subghz_view_receiver_add_item_to_menu(
            subghz->subghz_receiver,
            furi_string_get_cstr(item_name),
            furi_string_get_cstr(item_time),
            subghz_history_get_type_protocol(history, i),
            subghz_history_get_repeats(history, i));
        subghz_rx_key_state_set(subghz, SubGhzRxKeyStateAddKey);
    }
    furi_string_free(item_name);
    furi_string_free(item_time);

    subghz_view_receiver_set_callback(
        subghz->subghz_receiver, subghz_scene_receiver_callback, subghz);
    subghz_txrx_set_rx_callback(subghz->txrx, subghz_scene_add_to_history_callback, subghz);

    if(!subghz_history_full(subghz->history)) {
        subghz->state_notifications = SubGhzNotificationStateRx;
    } else {
        subghz->state_notifications = SubGhzNotificationStateIDLE;
    }

    // Check if hopping was enabled
    if(subghz->last_settings->enable_hopping) {
        subghz_txrx_hopper_set_state(subghz->txrx, SubGhzHopperStateRunning);
    } else {
        subghz_txrx_hopper_set_state(subghz->txrx, SubGhzHopperStateOFF);
    }

    // Check if Sound was enabled, and restart the Speaker.
    subghz_txrx_speaker_set_state(
        subghz->txrx,
        subghz->last_settings->enable_sound ? SubGhzSpeakerStateEnable :
                                              SubGhzSpeakerStateShutdown);

    //Set up a timer for the repeater (recycled favorites timeout TX timer!).
    if(!subghz->timer)
        subghz->timer = furi_timer_alloc(repeater_stop_callback, FuriTimerTypeOnce, subghz);

    //Remember if the repeater was loaded, and do cleanups we need.
    subghz->repeater = subghz->last_settings->repeater_state;

    //Did the user set BinRAW or me?
    if(subghz->repeater != SubGhzRepeaterStateOff) {
        //User had BinRAW on if the last settings had BinRAW on, if not, repeater is on, and BinRAW goes on, but State CHanged is false!
        subghz->repeater_bin_raw_was_off =
            (subghz->last_settings->filter & SubGhzProtocolFlag_BinRAW) == 0;
        if((subghz->filter & SubGhzProtocolFlag_BinRAW) == 0) {
            subghz->filter = SubGhzProtocolFlag_Decodable | SubGhzProtocolFlag_BinRAW;
            subghz_txrx_receiver_set_filter(subghz->txrx, subghz->filter);
        }
    } else {
        subghz->repeater_bin_raw_was_off = false;
    }

    subghz_txrx_rx_start(subghz->txrx);
    subghz_view_receiver_set_idx_menu(subghz->subghz_receiver, subghz->idx_menu_chosen);

    //to use a universal decoder, we are looking for a link to it
    furi_check(
        subghz_txrx_load_decoder_by_name_protocol(subghz->txrx, SUBGHZ_PROTOCOL_BIN_RAW_NAME));

    subghz_scene_receiver_update_statusbar(subghz);

    subghz_view_receiver_set_lock(subghz->subghz_receiver, subghz_is_locked(subghz));

    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewIdReceiver);
}

bool subghz_scene_receiver_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        // Save cursor position before going to any other dialog
        subghz->idx_menu_chosen = subghz_view_receiver_get_idx_menu(subghz->subghz_receiver);

        switch(event.event) {
        case SubGhzCustomEventViewReceiverBack:
            // Stop CC1101 Rx
            subghz->state_notifications = SubGhzNotificationStateIDLE;
            subghz_txrx_stop(subghz->txrx);
            subghz_txrx_hopper_set_state(subghz->txrx, SubGhzHopperStateOFF);
            subghz_txrx_set_rx_callback(subghz->txrx, NULL, subghz);

            if(subghz_history_get_last_index(subghz->history)) {
                subghz_rx_key_state_set(subghz, SubGhzRxKeyStateExit);
                scene_manager_next_scene(subghz->scene_manager, SubGhzSceneNeedSaving);
            } else {
                subghz_rx_key_state_set(subghz, SubGhzRxKeyStateIDLE);
                subghz_txrx_set_default_preset(subghz->txrx, subghz->last_settings->frequency);
                scene_manager_search_and_switch_to_previous_scene(
                    subghz->scene_manager, SubGhzSceneStart);
            }
            consumed = true;
            break;
        case SubGhzCustomEventViewReceiverOK:
            // Show file info, scene: receiver_info
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneReceiverInfo);
            dolphin_deed(DolphinDeedSubGhzReceiverInfo);
            consumed = true;
            break;
        case SubGhzCustomEventViewReceiverDeleteItem:
            subghz->state_notifications = SubGhzNotificationStateRx;

            subghz_view_receiver_disable_draw_callback(subghz->subghz_receiver);

            subghz_history_delete_item(subghz->history, subghz->idx_menu_chosen);
            subghz_view_receiver_delete_item(
                subghz->subghz_receiver,
                subghz_view_receiver_get_idx_menu(subghz->subghz_receiver));
            subghz_view_receiver_enable_draw_callback(subghz->subghz_receiver);

            subghz_scene_receiver_update_statusbar(subghz);
            subghz->idx_menu_chosen = subghz_view_receiver_get_idx_menu(subghz->subghz_receiver);
            if(subghz_history_get_last_index(subghz->history) == 0) {
                subghz_rx_key_state_set(subghz, SubGhzRxKeyStateStart);
            }
            consumed = true;
            break;
        case SubGhzCustomEventViewReceiverConfig:
            // Actually signals are received but SubGhzNotificationStateRx is not working inside Config Scene
            scene_manager_set_scene_state(
                subghz->scene_manager, SubGhzViewIdReceiver, SubGhzCustomEventManagerSet);
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneReceiverConfig);
            consumed = true;
            break;
        case SubGhzCustomEventViewReceiverOffDisplay:
            notification_message(subghz->notifications, &sequence_display_backlight_off);
            consumed = true;
            break;
        case SubGhzCustomEventViewReceiverUnlock:
            subghz_unlock(subghz);
            consumed = true;
            break;
        case SubGhzCustomEventViewRepeaterStart:
            subghz_txrx_stop(subghz->txrx);
            subghz_txrx_hopper_pause(subghz->txrx);

            FlipperFormat* key_repeat_data = subghz_history_get_raw_data(
                subghz->history, subghz_history_get_last_index(subghz->history) - 1);

            uint32_t tmpTe = 300;
            if(!flipper_format_rewind(key_repeat_data)) {
                FURI_LOG_E(TAG, "Rewind error");
            } else if(!flipper_format_read_uint32(key_repeat_data, "TE", (uint32_t*)&tmpTe, 1)) {
                FURI_LOG_E(TAG, "Missing TE");
            }

            if(subghz_txrx_tx_start(subghz->txrx, key_repeat_data) != SubGhzTxRxStartTxStateOk) {
                view_dispatcher_send_custom_event(
                    subghz->view_dispatcher, SubGhzCustomEventViewRepeaterStop);
            } else {
                subghz->state_notifications = SubGhzNotificationStateTx;
                notification_message(subghz->notifications, &subghz_sequence_tx_beep);

                uint32_t repeatnormal = (tmpTe > 1000) ? 1 : 3;
                uint32_t repeat_time = ((subghz->repeater & SubGhzRepeaterStateOnLong) != 0) ?
                                           2 * repeatnormal * tmpTe :
                                       ((subghz->repeater & SubGhzRepeaterStateOnShort) != 0) ?
                                           1 * tmpTe :
                                           repeatnormal * tmpTe;
                furi_timer_start(subghz->timer, repeat_time);
            }
            subghz_rx_key_state_set(subghz, SubGhzRxKeyStateTX);
            break;
        case SubGhzCustomEventViewRepeaterStop:
            subghz_txrx_stop(subghz->txrx);
            subghz_history_delete_item(
                subghz->history, subghz_history_get_last_index(subghz->history) - 1);
            subghz_txrx_rx_start(subghz->txrx);
            subghz_txrx_hopper_unpause(subghz->txrx);
            subghz_rx_key_state_set(subghz, SubGhzRxKeyStateStart);
            subghz->state_notifications = SubGhzNotificationStateRx;
            break;
        case SubGhzCustomEventViewReceiverOKLong:
            subghz_txrx_stop(subghz->txrx);
            subghz_txrx_hopper_pause(subghz->txrx);
            if(subghz_txrx_tx_start(
                   subghz->txrx,
                   subghz_history_get_raw_data(
                       subghz->history,
                       subghz_view_receiver_get_idx_menu(subghz->subghz_receiver))) !=
               SubGhzTxRxStartTxStateOk) {
                view_dispatcher_send_custom_event(
                    subghz->view_dispatcher, SubGhzCustomEventViewReceiverOKRelease);
            } else {
                subghz->state_notifications = SubGhzNotificationStateTx;
                notification_message(subghz->notifications, &subghz_sequence_tx_beep);
            }
            subghz_rx_key_state_set(subghz, SubGhzRxKeyStateTX);
            break;
        case SubGhzCustomEventViewReceiverOKRelease:
            if(subghz_rx_key_state_get(subghz) == SubGhzRxKeyStateTX) {
                subghz_txrx_stop(subghz->txrx);
                subghz_txrx_rx_start(subghz->txrx);
                subghz_txrx_hopper_unpause(subghz->txrx);
                subghz_rx_key_state_set(subghz, SubGhzRxKeyStateStart);
                subghz->state_notifications = SubGhzNotificationStateRx;
                break;
            }
            break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        if(subghz_rx_key_state_get(subghz) != SubGhzRxKeyStateTX) {
            if(subghz_txrx_hopper_get_state(subghz->txrx) != SubGhzHopperStateOFF) {
                subghz_txrx_hopper_update(subghz->txrx);
                subghz_scene_receiver_update_statusbar(subghz);
            }

            SubGhzThresholdRssiData ret_rssi = subghz_threshold_get_rssi_data(
                subghz->threshold_rssi, subghz_txrx_radio_device_get_rssi(subghz->txrx));

            if(subghz->last_settings->gps_baudrate != 0) {
                FuriHalRtcDateTime datetime;
                furi_hal_rtc_get_datetime(&datetime);
                if((datetime.second - subghz->gps->fix_second) > 15) {
                    subghz->gps->latitude = NAN;
                    subghz->gps->longitude = NAN;
                    subghz->gps->satellites = 0;
                    subghz->gps->fix_hour = 0;
                    subghz->gps->fix_minute = 0;
                    subghz->gps->fix_second = 0;
                }
                subghz_scene_receiver_update_statusbar(subghz);
            }

            subghz_receiver_rssi(subghz->subghz_receiver, ret_rssi.rssi);
            subghz_protocol_decoder_bin_raw_data_input_rssi(
                (SubGhzProtocolDecoderBinRAW*)subghz_txrx_get_decoder(subghz->txrx),
                ret_rssi.rssi);
        }

        switch(subghz->state_notifications) {
        case SubGhzNotificationStateRx:
            if(subghz->last_settings->gps_baudrate != 0) {
                if(subghz->gps->satellites > 0) {
                    notification_message(subghz->notifications, &sequence_blink_green_10);
                } else {
                    notification_message(subghz->notifications, &sequence_blink_red_10);
                }
            } else {
                notification_message(subghz->notifications, &sequence_blink_cyan_10);
            }
            break;
        case SubGhzNotificationStateRxDone:
            if(!subghz_is_locked(subghz)) {
                notification_message(subghz->notifications, &subghz_sequence_rx);
            } else {
                notification_message(subghz->notifications, &subghz_sequence_rx_locked);
            }
            subghz->state_notifications = SubGhzNotificationStateRx;
            break;
        case SubGhzNotificationStateTx:
            notification_message(subghz->notifications, &sequence_blink_magenta_10);
            break;
        default:
            break;
        }
    }
    return consumed;
}

void subghz_scene_receiver_on_exit(void* context) {
    UNUSED(context);
}
