#include "../subghz_i.h"
#include "../views/receiver.h"

#include <lib/subghz/subghz_file_encoder_worker.h>

#define TAG "SubGhzDecodeRaw"

// TODO:
// [ ] Decode in tick events instead of on_enter
// [ ] Make "Config" label optional in subghz_view_receiver_draw (../views/receiver.c)
// [ ] Make "Scanning..." label optional in subghz_view_receiver_draw (../views/receiver.c)
// [ ] Stop rx blink (blue, fast) on history item view
// [X] Don't reparse file on back

typedef enum{
	SubGhzDecodeRawStateNew,
	SubGhzDecodeRawStateLoading,
	SubGhzDecodeRawStateLoaded,
} SubGhzDecodeRawState;

SubGhzDecodeRawState decode_raw_state = SubGhzDecodeRawStateNew;

static void subghz_scene_receiver_update_statusbar(void* context) {
    SubGhz* subghz = context;
    string_t history_stat_str;
    string_init(history_stat_str);
    if(!subghz_history_get_text_space_left(subghz->txrx->history, history_stat_str)) {
        string_t frequency_str;
        string_t modulation_str;

        string_init(frequency_str);
        string_init(modulation_str);

        subghz_get_frequency_modulation(subghz, frequency_str, modulation_str);

        subghz_view_receiver_add_data_statusbar(
            subghz->subghz_receiver,
            string_get_cstr(frequency_str),
            string_get_cstr(modulation_str),
            string_get_cstr(history_stat_str));

        string_clear(frequency_str);
        string_clear(modulation_str);
    } else {
        subghz_view_receiver_add_data_statusbar(
            subghz->subghz_receiver, string_get_cstr(history_stat_str), "", "");
        subghz->state_notifications = SubGhzNotificationStateIDLE;
    }
    string_clear(history_stat_str);
}

void subghz_scene_decode_raw_callback(SubGhzCustomEvent event, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    view_dispatcher_send_custom_event(subghz->view_dispatcher, event);
}

static void subghz_scene_add_to_history_callback(
    SubGhzReceiver* receiver,
    SubGhzProtocolDecoderBase* decoder_base,
    void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    string_t str_buff;
    string_init(str_buff);

    if(subghz_history_add_to_history(subghz->txrx->history, decoder_base, subghz->txrx->preset)) {
        string_reset(str_buff);

        subghz->state_notifications = SubGhzNotificationStateRxDone;

        subghz_history_get_text_item_menu(
            subghz->txrx->history, str_buff, subghz_history_get_item(subghz->txrx->history) - 1);
		FURI_LOG_D(TAG, "Item: %s", string_get_cstr(str_buff));
        subghz_view_receiver_add_item_to_menu(
            subghz->subghz_receiver,
            string_get_cstr(str_buff),
            subghz_history_get_type_protocol(
                subghz->txrx->history, subghz_history_get_item(subghz->txrx->history) - 1));

        subghz_scene_receiver_update_statusbar(subghz);
    }
    subghz_receiver_reset(receiver);
    string_clear(str_buff);
    subghz->txrx->rx_key_state = SubGhzRxKeyStateAddKey;
}

bool subghz_scene_decode_raw(SubGhz* subghz) {
	string_t file_name;
	string_init(file_name);
	bool success = false;
	do {
		if(!flipper_format_rewind(subghz->txrx->fff_data)) {
			FURI_LOG_E(TAG, "Rewind error");
			break;
		}

		if(!flipper_format_read_string(subghz->txrx->fff_data, "File_name", file_name)) {
			FURI_LOG_E(TAG, "Missing File_name");
			break;
		}

		success = true;
	} while(false);

	if(!success) {
		return false;
	}

	FURI_LOG_I(TAG,
		"Listening at \033[0;33m%s\033[0m.",
		string_get_cstr(file_name));

	SubGhzFileEncoderWorker* file_worker_encoder = subghz_file_encoder_worker_alloc();
	if(subghz_file_encoder_worker_start(file_worker_encoder, string_get_cstr(file_name))) {
		//the worker needs a file in order to open and read part of the file
		furi_delay_ms(100);
	}

	LevelDuration level_duration;
	while(true) { // TODO: allow interrupt
		furi_delay_us(500); //you need to have time to read from the file from the SD card
		level_duration = subghz_file_encoder_worker_get_level_duration(file_worker_encoder);
		if(!level_duration_is_reset(level_duration)) {
			bool level = level_duration_get_level(level_duration);
			uint32_t duration = level_duration_get_duration(level_duration);
			subghz_receiver_decode(subghz->txrx->receiver, level, duration);
		} else {
			decode_raw_state = SubGhzDecodeRawStateLoaded;
			break;
		}
	}

	if(subghz_file_encoder_worker_is_running(file_worker_encoder)) {
		subghz_file_encoder_worker_stop(file_worker_encoder);
	}
	subghz_file_encoder_worker_free(file_worker_encoder);

	string_clear(file_name);
	return true;
}

void subghz_scene_decode_raw_on_enter(void* context) {
    SubGhz* subghz = context;

    string_t str_buff;
    string_init(str_buff);

	//subghz->txrx->history
	//subghz->txrx->environment
	//subghz->txrx->receiver

	FURI_LOG_D(TAG, "on_enter, state: %d", decode_raw_state);

    subghz_view_receiver_set_lock(subghz->subghz_receiver, subghz->lock);
	subghz_view_receiver_set_callback(
        subghz->subghz_receiver, subghz_scene_decode_raw_callback, subghz);

    subghz_scene_receiver_update_statusbar(subghz);
	
	subghz_receiver_set_rx_callback(
		subghz->txrx->receiver, subghz_scene_add_to_history_callback, subghz);

	if(decode_raw_state == SubGhzDecodeRawStateNew) {
		//Decode RAW to history
		decode_raw_state = SubGhzDecodeRawStateLoading;
		subghz_history_reset(subghz->txrx->history);
		subghz_scene_decode_raw(subghz);
	} else {
		//Load history to receiver
		subghz_view_receiver_exit(subghz->subghz_receiver);
		for(uint8_t i = 0; i < subghz_history_get_item(subghz->txrx->history); i++) {
			string_reset(str_buff);
			subghz_history_get_text_item_menu(subghz->txrx->history, str_buff, i);
			subghz_view_receiver_add_item_to_menu(
				subghz->subghz_receiver,
				string_get_cstr(str_buff),
				subghz_history_get_type_protocol(subghz->txrx->history, i));
			subghz->txrx->rx_key_state = SubGhzRxKeyStateAddKey;
		}
		string_clear(str_buff);
		subghz_view_receiver_set_idx_menu(subghz->subghz_receiver, subghz->txrx->idx_menu_chosen);
	}

    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewIdReceiver);
}

bool subghz_scene_decode_raw_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
		//TODO: remove debug log
		FURI_LOG_D(TAG, "CustomEvent: %d", event.event);
        switch(event.event) {
        case SubGhzCustomEventViewReceiverBack:
			decode_raw_state = SubGhzDecodeRawStateNew;
            subghz->txrx->idx_menu_chosen = 0;
            subghz_receiver_set_rx_callback(subghz->txrx->receiver, NULL, subghz);

			scene_manager_search_and_switch_to_previous_scene(
				subghz->scene_manager, SubGhzSceneMoreRAW);
            consumed = true;
            break;
        case SubGhzCustomEventViewReceiverOK:
            subghz->txrx->idx_menu_chosen =
                subghz_view_receiver_get_idx_menu(subghz->subghz_receiver);
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneReceiverInfo);
            consumed = true;
            break;
        case SubGhzCustomEventViewReceiverConfig:
			FURI_LOG_I(TAG, "No config options");
            consumed = true;
            break;
        case SubGhzCustomEventViewReceiverOffDisplay:
            notification_message(subghz->notifications, &sequence_display_backlight_off);
            consumed = true;
            break;
        case SubGhzCustomEventViewReceiverUnlock:
            subghz->lock = SubGhzLockOff;
            consumed = true;
            break;
        default:
            break;
        }
	} else if(event.type == SceneManagerEventTypeTick) {
		//TODO: event loop, handle reading here and give status updates
	} else if(event.type == SceneManagerEventTypeBack) {
		FURI_LOG_D(TAG, "BackEvent");
	}
    return consumed;
}

void subghz_scene_decode_raw_on_exit(void* context) {
    UNUSED(context);
}
