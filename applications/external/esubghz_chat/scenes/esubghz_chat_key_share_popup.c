#include "../esubghz_chat_i.h"
#include "../nfc_helpers.h"

struct ReplayDictNfcWriterContext {
    uint8_t* cur;
    uint8_t* max;
};

static bool replay_dict_nfc_writer(uint64_t run_id, uint32_t counter, void* context) {
    struct ReplayDictNfcWriterContext* ctx = (struct ReplayDictNfcWriterContext*)context;

    struct ReplayDictNfcEntry entry = {.run_id = run_id, .counter = __htonl(counter), .unused = 0};

    if(ctx->cur + sizeof(entry) > ctx->max) {
        return false;
    }

    memcpy(ctx->cur, &entry, sizeof(entry));
    ctx->cur += sizeof(entry);

    return true;
}

static void prepare_nfc_dev_data(ESubGhzChatState* state) {
    NfcDeviceData* dev_data = state->nfc_dev_data;

    dev_data->protocol = NfcDeviceProtocolMifareUl;
    furi_hal_random_fill_buf(dev_data->nfc_data.uid, 7);
    dev_data->nfc_data.uid_len = 7;
    dev_data->nfc_data.atqa[0] = 0x44;
    dev_data->nfc_data.atqa[1] = 0x00;
    dev_data->nfc_data.sak = 0x00;

    dev_data->mf_ul_data.type = MfUltralightTypeNTAG215;
    dev_data->mf_ul_data.version.header = 0x00;
    dev_data->mf_ul_data.version.vendor_id = 0x04;
    dev_data->mf_ul_data.version.prod_type = 0x04;
    dev_data->mf_ul_data.version.prod_subtype = 0x02;
    dev_data->mf_ul_data.version.prod_ver_major = 0x01;
    dev_data->mf_ul_data.version.prod_ver_minor = 0x00;
    dev_data->mf_ul_data.version.storage_size = 0x11;
    dev_data->mf_ul_data.version.protocol_type = 0x03;

    /* write key */
    crypto_ctx_get_key(state->crypto_ctx, dev_data->mf_ul_data.data);

    /* write the replay dict */
    struct ReplayDictNfcWriterContext wr_ctx = {
        .cur = dev_data->mf_ul_data.data + (KEY_BITS / 8),
        .max = dev_data->mf_ul_data.data + NFC_MAX_BYTES};

    size_t n_entries =
        crypto_ctx_dump_replay_dict(state->crypto_ctx, replay_dict_nfc_writer, &wr_ctx);

    /* calculate size of data, add 16 for config pages */
    dev_data->mf_ul_data.data_size =
        (KEY_BITS / 8) + (n_entries * sizeof(struct ReplayDictNfcEntry)) + (NFC_CONFIG_PAGES * 4);
}

/* Prepares the key share popup scene. */
void scene_on_enter_key_share_popup(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_enter_key_share_popup");

    furi_assert(context);
    ESubGhzChatState* state = context;

    popup_reset(state->nfc_popup);

    popup_disable_timeout(state->nfc_popup);

    popup_set_header(state->nfc_popup, "Sharing...", 67, 13, AlignLeft, AlignTop);
    popup_set_icon(state->nfc_popup, 0, 3, &I_NFC_dolphin_emulation_47x61);
    popup_set_text(state->nfc_popup, "Sharing\nKey via\nNFC", 90, 28, AlignCenter, AlignTop);

    prepare_nfc_dev_data(state);
    nfc_worker_start(
        state->nfc_worker, NfcWorkerStateMfUltralightEmulate, state->nfc_dev_data, NULL, NULL);

    notification_message(state->notification, &sequence_blink_start_magenta);

    view_dispatcher_switch_to_view(state->view_dispatcher, ESubGhzChatView_NfcPopup);
}

/* Handles scene manager events for the key share popup scene. */
bool scene_on_event_key_share_popup(void* context, SceneManagerEvent event) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_event_key_share_popup");

    furi_assert(context);
    ESubGhzChatState* state = context;

    UNUSED(state);
    UNUSED(event);

    return false;
}

/* Cleans up the key share popup scene. */
void scene_on_exit_key_share_popup(void* context) {
    FURI_LOG_T(APPLICATION_NAME, "scene_on_exit_key_share_popup");

    furi_assert(context);
    ESubGhzChatState* state = context;

    popup_reset(state->nfc_popup);

    notification_message(state->notification, &sequence_blink_stop);

    nfc_worker_stop(state->nfc_worker);

    crypto_explicit_bzero(state->nfc_dev_data->mf_ul_data.data, KEY_BITS / 8);
    memset(state->nfc_dev_data, 0, sizeof(NfcDeviceData));
}
