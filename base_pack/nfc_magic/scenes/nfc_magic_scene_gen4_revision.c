#include "../nfc_magic_app_i.h"

enum {
    NfcMagicSceneGen4RevisionStateCardSearch,
    NfcMagicSceneGen4RevisionStateCardFound,
};

NfcCommand nfc_mafic_scene_gen4_revision_poller_callback(Gen4PollerEvent event, void* context) {
    NfcMagicApp* instance = context;
    furi_assert(event.data);

    NfcCommand command = NfcCommandContinue;

    if(event.type == Gen4PollerEventTypeCardDetected) {
        view_dispatcher_send_custom_event(
            instance->view_dispatcher, NfcMagicCustomEventCardDetected);
    } else if(event.type == Gen4PollerEventTypeRequestMode) {
        event.data->request_mode.mode = Gen4PollerModeGetRevision;
    } else if(event.type == Gen4PollerEventTypeSuccess) {
        // Copy revision from event to main instance to display it on success scene
        memcpy(
            instance->gen4_revision_display,
            event.data->revision_data,
            sizeof(event.data->revision_data));

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

static void nfc_magic_scene_gen4_revision_setup_view(NfcMagicApp* instance) {
    Popup* popup = instance->popup;
    popup_reset(popup);
    uint32_t state =
        scene_manager_get_scene_state(instance->scene_manager, NfcMagicSceneGen4Revision);

    if(state == NfcMagicSceneGen4RevisionStateCardSearch) {
        popup_set_icon(instance->popup, 0, 8, &I_NFC_manual_60x50);
        popup_set_text(
            instance->popup, "Apply the\ncard\nto the back", 128, 32, AlignRight, AlignCenter);
    } else {
        popup_set_icon(popup, 12, 23, &I_Loading_24);
        popup_set_header(popup, "Reading\nDon't move...", 52, 32, AlignLeft, AlignCenter);
    }

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewPopup);
}

void nfc_magic_scene_gen4_revision_on_enter(void* context) {
    NfcMagicApp* instance = context;

    scene_manager_set_scene_state(
        instance->scene_manager,
        NfcMagicSceneGen4Revision,
        NfcMagicSceneGen4RevisionStateCardSearch);
    nfc_magic_scene_gen4_revision_setup_view(instance);

    nfc_magic_app_blink_start(instance);

    instance->gen4_poller = gen4_poller_alloc(instance->nfc);
    gen4_poller_set_password(instance->gen4_poller, instance->gen4_password);
    gen4_poller_start(
        instance->gen4_poller, nfc_mafic_scene_gen4_revision_poller_callback, instance);
}

bool nfc_magic_scene_gen4_revision_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcMagicCustomEventCardDetected) {
            scene_manager_set_scene_state(
                instance->scene_manager,
                NfcMagicSceneGen4Revision,
                NfcMagicSceneGen4RevisionStateCardFound);
            nfc_magic_scene_gen4_revision_setup_view(instance);
            consumed = true;
        } else if(event.event == NfcMagicCustomEventCardLost) {
            scene_manager_set_scene_state(
                instance->scene_manager,
                NfcMagicSceneGen4Revision,
                NfcMagicSceneGen4RevisionStateCardSearch);
            nfc_magic_scene_gen4_revision_setup_view(instance);
            consumed = true;
        } else if(event.event == NfcMagicCustomEventWorkerSuccess) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4ShowRev);
            consumed = true;
        } else if(event.event == NfcMagicCustomEventWorkerFail) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4Fail);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_magic_scene_gen4_revision_on_exit(void* context) {
    NfcMagicApp* instance = context;

    gen4_poller_stop(instance->gen4_poller);
    gen4_poller_free(instance->gen4_poller);
    scene_manager_set_scene_state(
        instance->scene_manager,
        NfcMagicSceneGen4Revision,
        NfcMagicSceneGen4RevisionStateCardSearch);
    // Clear view
    popup_reset(instance->popup);

    nfc_magic_app_blink_stop(instance);
}
