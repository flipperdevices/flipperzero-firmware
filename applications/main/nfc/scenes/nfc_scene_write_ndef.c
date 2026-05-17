#include "../nfc_app_i.h"
#include "../helpers/ndef_builder.h"

#include <nfc/protocols/mf_ultralight/mf_ultralight_poller.h>

typedef enum {
    NfcSceneWriteNdefStateSearch,
    NfcSceneWriteNdefStateWriting,
    NfcSceneWriteNdefStateSuccess,
    NfcSceneWriteNdefStateFail,
    NfcSceneWriteNdefStateWrongCard,
} NfcSceneWriteNdefState;

static NfcCommand nfc_scene_write_ndef_worker_callback(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.event_data);
    furi_assert(event.protocol == NfcProtocolMfUltralight);

    NfcCommand command = NfcCommandContinue;
    NfcApp* instance = context;
    MfUltralightPollerEvent* mfu_event = event.event_data;

    if(mfu_event->type == MfUltralightPollerEventTypeRequestMode) {
        mfu_event->data->poller_mode = MfUltralightPollerModeWrite;
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventCardDetected);
    } else if(mfu_event->type == MfUltralightPollerEventTypeAuthRequest) {
        mfu_event->data->auth_context.skip_auth = true;
    } else if(mfu_event->type == MfUltralightPollerEventTypeRequestWriteData) {
        mfu_event->data->write_data =
            nfc_device_get_data(instance->nfc_device, NfcProtocolMfUltralight);
    } else if(mfu_event->type == MfUltralightPollerEventTypeCardMismatch) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventWrongCard);
        command = NfcCommandStop;
    } else if(mfu_event->type == MfUltralightPollerEventTypeCardLocked) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventPollerFailure);
        command = NfcCommandStop;
    } else if(mfu_event->type == MfUltralightPollerEventTypeWriteFail) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventPollerFailure);
        command = NfcCommandStop;
    } else if(mfu_event->type == MfUltralightPollerEventTypeWriteSuccess) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventPollerSuccess);
        command = NfcCommandStop;
    }
    return command;
}

static void nfc_scene_write_ndef_popup_callback(void* context) {
    NfcApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventViewExit);
}

static void nfc_scene_write_ndef_setup_view(NfcApp* instance) {
    Popup* popup = instance->popup;
    popup_reset(popup);
    uint32_t state = scene_manager_get_scene_state(instance->scene_manager, NfcSceneWriteNdef);

    if(state == NfcSceneWriteNdefStateSearch) {
        popup_set_header(popup, "Writing", 95, 20, AlignCenter, AlignCenter);
        popup_set_text(popup, "Apply the\ntarget tag", 95, 38, AlignCenter, AlignCenter);
        popup_set_icon(popup, 0, 8, &I_NFC_manual_60x50);
    } else if(state == NfcSceneWriteNdefStateWriting) {
        popup_set_header(popup, "Writing\nDon't move...", 52, 32, AlignLeft, AlignCenter);
        popup_set_icon(popup, 12, 23, &A_Loading_24);
    } else if(state == NfcSceneWriteNdefStateSuccess) {
        notification_message(instance->notifications, &sequence_success);
        popup_set_icon(popup, 48, 6, &I_DolphinDone_80x58);
        popup_set_header(popup, "Successfully\nwritten", 5, 22, AlignLeft, AlignBottom);
        popup_set_timeout(popup, 1500);
        popup_set_context(popup, instance);
        popup_set_callback(popup, nfc_scene_write_ndef_popup_callback);
        popup_enable_timeout(popup);
    } else if(state == NfcSceneWriteNdefStateFail) {
        notification_message(instance->notifications, &sequence_error);
        popup_set_icon(popup, 72, 17, &I_WarningDolphinFlip_45x42);
        popup_set_header(popup, "Write failed", 64, 4, AlignCenter, AlignTop);
        popup_set_text(
            popup,
            "Tag may be\nlocked or moved",
            8,
            22,
            AlignLeft,
            AlignTop);
        popup_set_timeout(popup, 2000);
        popup_set_context(popup, instance);
        popup_set_callback(popup, nfc_scene_write_ndef_popup_callback);
        popup_enable_timeout(popup);
    } else if(state == NfcSceneWriteNdefStateWrongCard) {
        notification_message(instance->notifications, &sequence_error);
        popup_set_icon(popup, 72, 17, &I_WarningDolphinFlip_45x42);
        popup_set_header(popup, "Wrong tag type", 64, 4, AlignCenter, AlignTop);
        popup_set_text(
            popup,
            "Use selected\nNTAG variant",
            8,
            22,
            AlignLeft,
            AlignTop);
        popup_set_timeout(popup, 2000);
        popup_set_context(popup, instance);
        popup_set_callback(popup, nfc_scene_write_ndef_popup_callback);
        popup_enable_timeout(popup);
    }

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewPopup);
}

static bool nfc_scene_write_ndef_build_payload(NfcApp* instance) {
    NdefWriteContext* ctx = &instance->ndef_write;
    uint8_t blob[NDEF_BUILDER_MAX_SIZE];
    size_t blob_size = 0;
    bool built = false;

    switch(ctx->record_type) {
    case NdefWriteRecordTypeUri:
        built = ndef_builder_build_uri(
            furi_string_get_cstr(ctx->primary), blob, sizeof(blob), &blob_size);
        break;
    case NdefWriteRecordTypeEmail: {
        // mailto: prefix is in the URI prefix table (code 0x06), so the builder
        // strips it from the on-tag payload automatically.
        FuriString* uri = furi_string_alloc_printf(
            "mailto:%s", furi_string_get_cstr(ctx->primary));
        built = ndef_builder_build_uri(furi_string_get_cstr(uri), blob, sizeof(blob), &blob_size);
        furi_string_free(uri);
        break;
    }
    case NdefWriteRecordTypePhone: {
        // tel: prefix is in the URI prefix table (code 0x05).
        FuriString* uri = furi_string_alloc_printf(
            "tel:%s", furi_string_get_cstr(ctx->primary));
        built = ndef_builder_build_uri(furi_string_get_cstr(uri), blob, sizeof(blob), &blob_size);
        furi_string_free(uri);
        break;
    }
    case NdefWriteRecordTypeText:
        built = ndef_builder_build_text(
            furi_string_get_cstr(ctx->primary), blob, sizeof(blob), &blob_size);
        break;
    case NdefWriteRecordTypeWifi:
        built = ndef_builder_build_wifi_wpa2(
            furi_string_get_cstr(ctx->primary),
            furi_string_get_cstr(ctx->secondary),
            blob,
            sizeof(blob),
            &blob_size);
        break;
    }

    if(!built) return false;
    return mf_ultralight_ndef_fill_device(instance->nfc_device, ctx->ntag_type, blob, blob_size);
}

void nfc_scene_write_ndef_on_enter(void* context) {
    NfcApp* instance = context;
    dolphin_deed(DolphinDeedNfcEmulate);

    // Clear any stale poller pointer left by a previous scene so on_exit can
    // tell whether *this* scene allocated one.
    instance->poller = NULL;

    if(!nfc_scene_write_ndef_build_payload(instance)) {
        scene_manager_set_scene_state(
            instance->scene_manager, NfcSceneWriteNdef, NfcSceneWriteNdefStateFail);
        nfc_scene_write_ndef_setup_view(instance);
        return;
    }

    scene_manager_set_scene_state(
        instance->scene_manager, NfcSceneWriteNdef, NfcSceneWriteNdefStateSearch);
    nfc_scene_write_ndef_setup_view(instance);

    instance->poller = nfc_poller_alloc(instance->nfc, NfcProtocolMfUltralight);
    nfc_poller_start(instance->poller, nfc_scene_write_ndef_worker_callback, instance);

    nfc_blink_emulate_start(instance);
}

bool nfc_scene_write_ndef_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventCardDetected) {
            scene_manager_set_scene_state(
                instance->scene_manager, NfcSceneWriteNdef, NfcSceneWriteNdefStateWriting);
            nfc_scene_write_ndef_setup_view(instance);
            consumed = true;
        } else if(event.event == NfcCustomEventWrongCard) {
            scene_manager_set_scene_state(
                instance->scene_manager, NfcSceneWriteNdef, NfcSceneWriteNdefStateWrongCard);
            nfc_scene_write_ndef_setup_view(instance);
            consumed = true;
        } else if(event.event == NfcCustomEventPollerSuccess) {
            scene_manager_set_scene_state(
                instance->scene_manager, NfcSceneWriteNdef, NfcSceneWriteNdefStateSuccess);
            nfc_scene_write_ndef_setup_view(instance);
            consumed = true;
        } else if(event.event == NfcCustomEventPollerFailure) {
            scene_manager_set_scene_state(
                instance->scene_manager, NfcSceneWriteNdef, NfcSceneWriteNdefStateFail);
            nfc_scene_write_ndef_setup_view(instance);
            consumed = true;
        } else if(event.event == NfcCustomEventViewExit) {
            consumed = scene_manager_search_and_switch_to_previous_scene(
                instance->scene_manager, NfcSceneStart);
        }
    }
    return consumed;
}

void nfc_scene_write_ndef_on_exit(void* context) {
    NfcApp* instance = context;

    if(instance->poller) {
        nfc_poller_stop(instance->poller);
        nfc_poller_free(instance->poller);
        instance->poller = NULL;
    }

    scene_manager_set_scene_state(
        instance->scene_manager, NfcSceneWriteNdef, NfcSceneWriteNdefStateSearch);
    popup_reset(instance->popup);
    nfc_blink_stop(instance);
}
