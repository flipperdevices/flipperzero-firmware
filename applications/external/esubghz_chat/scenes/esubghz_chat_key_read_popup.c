#include "../esubghz_chat_i.h"

typedef enum {
    KeyReadPopupState_Idle,
    KeyReadPopupState_Detecting,
    KeyReadPopupState_Reading,
} KeyReadPopupState;

static bool read_worker_cb(NfcWorkerEvent event, void* context) {
    furi_assert(context);
    ESubGhzChatState* state = context;

    view_dispatcher_send_custom_event(state->view_dispatcher, event);

    return true;
}

static bool key_read_popup_handle_key_read(ESubGhzChatState* state) {
    NfcDeviceData* dev_data = state->nfc_dev_data;

    if(dev_data->mf_ul_data.data_read < KEY_BITS / 8) {
        return false;
    }

    /* initiate the crypto context */
    bool ret = crypto_ctx_set_key(state->crypto_ctx, dev_data->mf_ul_data.data);

    /* cleanup */
    crypto_explicit_bzero(dev_data->mf_ul_data.data, KEY_BITS / 8);

    if(!ret) {
        crypto_ctx_clear(state->crypto_ctx);
        return false;
    }

    /* set encrypted flag and enter the chat */
    state->encrypted = true;
    enter_chat(state);

    return true;
}

static void key_read_popup_set_state(ESubGhzChatState* state, KeyReadPopupState new_state) {
    uint32_t cur_state =
        scene_manager_get_scene_state(state->scene_manager, ESubGhzChatScene_KeyReadPopup);
    if(cur_state == new_state) {
        return;
    }

    if(new_state == KeyReadPopupState_Detecting) {
        popup_reset(state->nfc_popup);
        popup_set_text(
            state->nfc_popup,
            "Apply card to\nFlipper's "
            "back",
            97,
            24,
            AlignCenter,
            AlignTop);
        popup_set_icon(state->nfc_popup, 0, 8, &I_NFC_manual_60x50);
    } else if(new_state == KeyReadPopupState_Reading) {
        popup_reset(state->nfc_popup);
        popup_set_header(
            state->nfc_popup,
            "Reading card\nDon't "
            "move...",
            85,
            24,
            AlignCenter,
            AlignTop);
        popup_set_icon(state->nfc_popup, 12, 23, &I_Loading_24);
    }

    scene_manager_set_scene_state(state->scene_manager, ESubGhzChatScene_KeyReadPopup, new_state);
}

/* Prepares the key share read scene. */
void scene_on_enter_key_read_popup(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_key_read_popup");

    furi_assert(context);
    ESubGhzChatState* state = context;

    key_read_popup_set_state(state, KeyReadPopupState_Detecting);

    state->nfc_dev_data->parsed_data = furi_string_alloc();
    if(state->nfc_dev_data->parsed_data == NULL) {
        /* can't do anything here, crash */
        furi_check(0);
    }

    nfc_worker_start(
        state->nfc_worker, NfcWorkerStateRead, state->nfc_dev_data, read_worker_cb, state);

    notification_message(state->notification, &sequence_blink_start_cyan);

    view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_NfcPopup);
}

/* Handles scene manager events for the key read popup scene. */
bool scene_on_event_key_read_popup(void* context, SceneManagerEvent event) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_event_key_read_popup");

    furi_assert(context);
    ESubGhzChatState* state = context;

    bool consumed = false;

    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        /* card detected */
        case NfcWorkerEventCardDetected:
            key_read_popup_set_state(state, KeyReadPopupState_Reading);
            notification_message(state->notification, &sequence_blink_start_yellow);
            consumed = true;
            break;

        /* no card detected */
        case NfcWorkerEventNoCardDetected:
            key_read_popup_set_state(state, KeyReadPopupState_Detecting);
            notification_message(state->notification, &sequence_blink_start_cyan);
            consumed = true;
            break;

        /* key probably read */
        case NfcWorkerEventReadMfUltralight:
            if(key_read_popup_handle_key_read(state)) {
                scene_manager_next_scene(state->scene_manager, ESubGhzChatScene_ChatInput);
            } else {
                if(!scene_manager_previous_scene(state->scene_manager)) {
                    view_dispatcher_stop(state->view_dispatcher);
                }
            }
            consumed = true;
            break;

        default:
            if(!scene_manager_previous_scene(state->scene_manager)) {
                view_dispatcher_stop(state->view_dispatcher);
            }
            consumed = true;
            break;
        }

        break;

    default:
        consumed = false;
        break;
    }

    return consumed;
}

/* Cleans up the key read popup scene. */
void scene_on_exit_key_read_popup(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_key_read_popup");

    furi_assert(context);
    ESubGhzChatState* state = context;

    popup_reset(state->nfc_popup);
    scene_manager_set_scene_state(
        state->scene_manager, ESubGhzChatScene_KeyReadPopup, KeyReadPopupState_Idle);

    notification_message(state->notification, &sequence_blink_stop);

    nfc_worker_stop(state->nfc_worker);

    crypto_explicit_bzero(state->nfc_dev_data->mf_ul_data.data, KEY_BITS / 8);
    if(state->nfc_dev_data->parsed_data != NULL) {
        furi_string_free(state->nfc_dev_data->parsed_data);
    }
    memset(state->nfc_dev_data, 0, sizeof(NfcDeviceData));
}
