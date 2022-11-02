#include "../nfc_magic_i.h"

enum {
    NfcMagicSceneWriteStateCardSearch,
    NfcMagicSceneWriteStateCardFound,
};

// bool nfc_magic_write_worker_callback(NfcMagicWorkerEvent event, void* context) {
//     furi_assert(context);

//     NfcMagic* nfc_magic = context;
//     view_dispatcher_send_custom_event(nfc->view_dispatcher, event);

//     return true;
// }

static void nfc_magic_scene_write_setup_view(NfcMagic* nfc_magic) {
    Popup* popup = nfc_magic->popup;
    popup_reset(popup);
    uint32_t state = scene_manager_get_scene_state(nfc_magic->scene_manager, NfcMagicSceneWrite);

    if(state == NfcMagicSceneWriteStateCardSearch) {
        popup_set_text(
            nfc_magic->popup, "Apply the initial\ncard only", 128, 32, AlignRight, AlignCenter);
        // popup_set_icon(nfc_magic->popup, 0, 8, &I_NFC_manual_60x50);
    } else {
        popup_set_header(popup, "Writing\nDon't move...", 52, 32, AlignLeft, AlignCenter);
        // popup_set_icon(popup, 12, 23, &A_Loading_24);
    }

    view_dispatcher_switch_to_view(nfc_magic->view_dispatcher, NfcMagicViewPopup);

    FURI_LOG_I("wupa", "send wupa");
    bool ret = magic_wupa();
    FURI_LOG_W("write", "return %d", ret);
    ret = magic_data_access_cmd();
    FURI_LOG_W("write cmd", "return %d", ret);
    ret = magic_write_blk(1, &nfc_magic->nfc_dev->dev_data.mf_classic_data.block[1]);
    FURI_LOG_W("write", "return %d", ret);
    ret = magic_read_block(1, &nfc_magic->nfc_dev->dev_data.mf_classic_data.block[1]);
    FURI_LOG_W("read", "return %d", ret);
    magic_deactivate();
}

void nfc_magic_scene_write_on_enter(void* context) {
    NfcMagic* nfc_magic = context;

    scene_manager_set_scene_state(
        nfc_magic->scene_manager, NfcMagicSceneWrite, NfcMagicSceneWriteStateCardSearch);
    nfc_magic_scene_write_setup_view(nfc_magic);

    // Setup and start worker
    // nfc_worker_start(
    //     nfc->worker,
    //     NfcMagicWorkerStateMfClassicWrite,
    //     &nfc->dev->dev_data,
    //     nfc_magic_write_worker_callback,
    //     nfc);
    nfc_magic_blink_start(nfc_magic);
}

bool nfc_magic_scene_write_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    // NfcMagic* nfc_magic = context;
    bool consumed = false;

    // if(event.type == SceneManagerEventTypeCustom) {
    //     if(event.event == NfcMagicWorkerEventSuccess) {
    //         scene_manager_next_scene(nfc->scene_manager, NfcMagicSceneWriteSuccess);
    //         consumed = true;
    //     } else if(event.event == NfcMagicWorkerEventFail) {
    //         scene_manager_next_scene(nfc->scene_manager, NfcMagicSceneWriteFail);
    //         consumed = true;
    //     } else if(event.event == NfcMagicWorkerEventWrongCard) {
    //         scene_manager_next_scene(nfc->scene_manager, NfcMagicSceneMfClassicWrongCard);
    //         consumed = true;
    //     } else if(event.event == NfcMagicWorkerEventCardDetected) {
    //         scene_manager_set_scene_state(
    //             nfc->scene_manager, NfcMagicSceneWrite, NfcMagicSceneWriteStateCardFound);
    //         nfc_magic_scene_write_setup_view(nfc);
    //         consumed = true;
    //     } else if(event.event == NfcMagicWorkerEventNoCardDetected) {
    //         scene_manager_set_scene_state(
    //             nfc->scene_manager, NfcMagicSceneWrite, NfcMagicSceneWriteStateCardSearch);
    //         nfc_magic_scene_write_setup_view(nfc);
    //         consumed = true;
    //     }
    // }
    return consumed;
}

void nfc_magic_scene_write_on_exit(void* context) {
    NfcMagic* nfc_magic = context;

    // nfc_worker_stop(nfc->worker);
    scene_manager_set_scene_state(
        nfc_magic->scene_manager, NfcMagicSceneWrite, NfcMagicSceneWriteStateCardSearch);
    // Clear view
    popup_reset(nfc_magic->popup);

    nfc_magic_blink_stop(nfc_magic);
}
