#include "../nfc_magic_app_i.h"

enum {
    NfcMagicSceneWriteStateCardSearch,
    NfcMagicSceneWriteStateCardFound,
};

NfcCommand nfc_mafic_scene_write_gen1_poller_callback(Gen1aPollerEvent event, void* context) {
    NfcMagicApp* instance = context;
    furi_assert(event.data);

    NfcCommand command = NfcCommandContinue;

    if(event.type == Gen1aPollerEventTypeDetected) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicCustomEventCardDetected);
    } else if(event.type == Gen1aPollerEventTypeRequestMode) {
        event.data->request_mode.mode = Gen1aPollerModeWrite;
    } else if(event.type == Gen1aPollerEventTypeRequestDataToWrite) {
        const MfClassicData* mfc_data =
            nfc_device_get_data(instance->source_dev, NfcProtocolMfClassic);
        event.data->data_to_write.mfc_data = mfc_data;
    } else if(event.type == Gen1aPollerEventTypeSuccess) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicCustomEventWorkerSuccess);
        command = NfcCommandStop;
    } else if(event.type == Gen1aPollerEventTypeFail) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicCustomEventWorkerFail);
        command = NfcCommandStop;
    }

    return command;
}

NfcCommand nfc_mafic_scene_write_gen4_poller_callback(Gen4PollerEvent event, void* context) {
    NfcMagicApp* instance = context;
    furi_assert(event.data);

    NfcCommand command = NfcCommandContinue;

    if(event.type == Gen4PollerEventTypeCardDetected) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicCustomEventCardDetected);
    } else if(event.type == Gen4PollerEventTypeRequestMode) {
        event.data->request_mode.mode = Gen4PollerModeWrite;
    } else if(event.type == Gen4PollerEventTypeRequestDataToWrite) {
        NfcProtocol protocol = nfc_device_get_protocol(instance->source_dev);
        event.data->request_data.protocol = protocol;
        event.data->request_data.data = nfc_device_get_data(instance->source_dev, protocol);
    } else if(event.type == Gen4PollerEventTypeSuccess) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicCustomEventWorkerSuccess);
        command = NfcCommandStop;
    } else if(event.type == Gen4PollerEventTypeFail) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicCustomEventWorkerFail);
        command = NfcCommandStop;
    }

    return command;
}

static void nfc_magic_scene_write_setup_view(NfcMagicApp* instance) {
    Popup* popup = instance->popup;
    popup_reset(popup);
    uint32_t state = scene_manager_get_scene_state(instance->scene_manager, NfcMagicSceneWrite);

    if(state == NfcMagicSceneWriteStateCardSearch) {
        popup_set_icon(instance->popup, 0, 8, &I_NFC_manual_60x50);
        popup_set_text(
            instance->popup, "Apply the\nsame card\nto the back", 128, 32, AlignRight, AlignCenter);
    } else {
        popup_set_icon(popup, 12, 23, &I_Loading_24);
        popup_set_header(popup, "Writing\nDon't move...", 52, 32, AlignLeft, AlignCenter);
    }

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewPopup);
}

void nfc_magic_scene_write_on_enter(void* context) {
    NfcMagicApp* instance = context;

    scene_manager_set_scene_state(
        instance->scene_manager, NfcMagicSceneWrite, NfcMagicSceneWriteStateCardSearch);
    nfc_magic_scene_write_setup_view(instance);

    nfc_magic_app_blink_start(instance);

    if(instance->protocol == NfcMagicProtocolGen1) {
        instance->gen1a_poller = gen1a_poller_alloc(instance->nfc);
        gen1a_poller_start(
            instance->gen1a_poller, nfc_mafic_scene_write_gen1_poller_callback, instance);
    } else {
        instance->gen4_poller = gen4_poller_alloc(instance->nfc);
        gen4_poller_set_password(instance->gen4_poller, instance->gen4_password);
        gen4_poller_start(
            instance->gen4_poller, nfc_mafic_scene_write_gen4_poller_callback, instance);
    }
}

bool nfc_magic_scene_write_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcMagicCustomEventCardDetected) {
            scene_manager_set_scene_state(
                instance->scene_manager, NfcMagicSceneWrite, NfcMagicSceneWriteStateCardFound);
            nfc_magic_scene_write_setup_view(instance);
            consumed = true;
        } else if(event.event == NfcMagicCustomEventCardLost) {
            scene_manager_set_scene_state(
                instance->scene_manager, NfcMagicSceneWrite, NfcMagicSceneWriteStateCardSearch);
            nfc_magic_scene_write_setup_view(instance);
            consumed = true;
        } else if(event.event == NfcMagicCustomEventWorkerSuccess) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneSuccess);
            consumed = true;
        } else if(event.event == NfcMagicCustomEventWorkerFail) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneWriteFail);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_magic_scene_write_on_exit(void* context) {
    NfcMagicApp* instance = context;

    if(instance->protocol == NfcMagicProtocolGen1) {
        gen1a_poller_stop(instance->gen1a_poller);
        gen1a_poller_free(instance->gen1a_poller);
    } else {
        gen4_poller_stop(instance->gen4_poller);
        gen4_poller_free(instance->gen4_poller);
    }
    scene_manager_set_scene_state(
        instance->scene_manager, NfcMagicSceneWrite, NfcMagicSceneWriteStateCardSearch);
    // Clear view
    popup_reset(instance->popup);

    nfc_magic_app_blink_stop(instance);
}
