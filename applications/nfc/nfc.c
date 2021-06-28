#include "nfc_i.h"
#include "api-hal-nfc.h"
#include "app_scene.h"

Nfc* nfc_alloc() {
    Nfc* nfc = furi_alloc(sizeof(Nfc));

    nfc->nfc_common.worker = nfc_worker_alloc();
    nfc->nfc_common.view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(nfc->nfc_common.view_dispatcher);
    view_dispatcher_enable_navigation(nfc->nfc_common.view_dispatcher, nfc);
    nfc->nfc_common.worker_result = furi_alloc(sizeof(NfcWorkerResult));

    // Open GUI record
    nfc->gui = furi_record_open("gui");
    view_dispatcher_attach_to_gui(
        nfc->nfc_common.view_dispatcher, nfc->gui, ViewDispatcherTypeFullscreen);

    // Submenu
    nfc->submenu = submenu_alloc();
    view_dispatcher_add_view(
        nfc->nfc_common.view_dispatcher, NfcViewMenu, submenu_get_view(nfc->submenu));

    // Dialog
    nfc->dialog_ex = dialog_ex_alloc();
    view_dispatcher_add_view(
        nfc->nfc_common.view_dispatcher, NfcViewDialogEx, dialog_ex_get_view(nfc->dialog_ex));

    // Popup
    nfc->popup = popup_alloc();
    view_dispatcher_add_view(
        nfc->nfc_common.view_dispatcher, NfcViewPopup, popup_get_view(nfc->popup));

    // Text Input
    nfc->text_input = text_input_alloc();
    view_dispatcher_add_view(
        nfc->nfc_common.view_dispatcher, NfcViewTextInput, text_input_get_view(nfc->text_input));

    // Byte Input
    nfc->byte_input = byte_input_alloc();
    view_dispatcher_add_view(
        nfc->nfc_common.view_dispatcher, NfcViewByteInput, byte_input_get_view(nfc->byte_input));

    // Detect
    nfc->nfc_detect = nfc_detect_alloc(&nfc->nfc_common);
    view_dispatcher_add_view(
        nfc->nfc_common.view_dispatcher, NfcViewDetect, nfc_detect_get_view(nfc->nfc_detect));

    // Emulate
    nfc->nfc_emulate = nfc_emulate_alloc(&nfc->nfc_common);
    view_dispatcher_add_view(
        nfc->nfc_common.view_dispatcher, NfcViewEmulate, nfc_emulate_get_view(nfc->nfc_emulate));

    // EMV
    nfc->nfc_emv = nfc_emv_alloc(&nfc->nfc_common);
    view_dispatcher_add_view(
        nfc->nfc_common.view_dispatcher, NfcViewEmv, nfc_emv_get_view(nfc->nfc_emv));

    // Mifare Ultralight
    nfc->nfc_mifare_ul = nfc_mifare_ul_alloc(&nfc->nfc_common);
    view_dispatcher_add_view(
        nfc->nfc_common.view_dispatcher,
        NfcViewMifareUl,
        nfc_mifare_ul_get_view(nfc->nfc_mifare_ul));

    // Scene allocation
    nfc->scene_start = nfc_scene_start_alloc();
    nfc->scene_read_card = nfc_scene_read_card_alloc();
    nfc->scene_read_card_success = nfc_scene_read_card_success_alloc();
    nfc->scene_card_menu = nfc_scene_card_menu_alloc();
    nfc->scene_not_implemented = nfc_scene_not_implemented_alloc();

    view_dispatcher_add_scene(nfc->nfc_common.view_dispatcher, nfc->scene_start);

    return nfc;
}

void nfc_free(Nfc* nfc) {
    furi_assert(nfc);

    // Submenu
    view_dispatcher_remove_view(nfc->nfc_common.view_dispatcher, NfcViewMenu);
    submenu_free(nfc->submenu);

    // Detect
    view_dispatcher_remove_view(nfc->nfc_common.view_dispatcher, NfcViewDetect);
    nfc_detect_free(nfc->nfc_detect);

    // Emulate
    view_dispatcher_remove_view(nfc->nfc_common.view_dispatcher, NfcViewEmulate);
    nfc_emulate_free(nfc->nfc_emulate);

    // EMV
    view_dispatcher_remove_view(nfc->nfc_common.view_dispatcher, NfcViewEmv);
    nfc_emv_free(nfc->nfc_emv);

    // Mifare ultralight
    view_dispatcher_remove_view(nfc->nfc_common.view_dispatcher, NfcViewMifareUl);
    nfc_mifare_ul_free(nfc->nfc_mifare_ul);

    // Worker
    nfc_worker_stop(nfc->nfc_common.worker);
    nfc_worker_free(nfc->nfc_common.worker);

    // Scenes
    nfc_scene_start_free(nfc->scene_start);
    nfc_scene_read_card_free(nfc->scene_read_card);
    nfc_scene_read_card_success_alloc(nfc->scene_read_card_success);
    nfc_scene_card_menu_free(nfc->scene_card_menu);
    nfc_scene_not_implemented_free(nfc->scene_not_implemented);

    // View Dispatcher
    view_dispatcher_free(nfc->nfc_common.view_dispatcher);

    // GUI
    furi_record_close("gui");
    nfc->gui = NULL;

    free(nfc);
}

int32_t nfc_task(void* p) {
    Nfc* nfc = nfc_alloc();

    view_dispatcher_run(nfc->nfc_common.view_dispatcher);

    nfc_free(nfc);

    return 0;
}
