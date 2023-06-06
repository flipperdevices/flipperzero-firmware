#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

void nfc_scene_mf_classic_read_supported_card_on_enter(void* context) {
    NfcApp* nfc = context;

    // Setup view
    popup_reset(nfc->popup);
    popup_set_text(nfc->popup, "Apply card to\nFlipper's back", 97, 24, AlignCenter, AlignTop);
    popup_set_icon(nfc->popup, 0, 8, &I_NFC_manual_60x50);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);

    nfc_blink_read_start(nfc);

    bool supported = false;
    nfc->supported_cards = nfc_supported_cards_alloc();
    if(nfc_supported_cards_read(
           nfc->supported_cards,
           NfcDevProtocolMfClassic,
           nfc->mf_classic_poller,
           &nfc->nfc_dev_data.mf_classic_data)) {
        supported = nfc_supported_cards_parse(
            nfc->supported_cards,
            NfcDevProtocolMfClassic,
            &nfc->nfc_dev_data.mf_classic_data,
            nfc->parsed_data);
    }
    
    if(supported) {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneMfClassicReadSuccess);
    } else {
        scene_manager_next_scene(nfc->scene_manager, NfcSceneMfClassicDictAttack);
    }

    nfc_supported_cards_free(nfc->supported_cards);
}

bool nfc_scene_mf_classic_read_supported_card_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);

    return false;
}

void nfc_scene_mf_classic_read_supported_card_on_exit(void* context) {
    NfcApp* nfc = context;

    // Clear view
    popup_reset(nfc->popup);

    nfc_blink_stop(nfc);
}
