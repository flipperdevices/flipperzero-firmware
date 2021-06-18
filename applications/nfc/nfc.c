#include "nfc_i.h"
#include "api-hal-nfc.h"

uint32_t nfc_view_exit(void* context) {
    return VIEW_NONE;
}

void nfc_menu_callback(void* context, uint32_t index) {
    furi_assert(context);

    Nfc* nfc = (Nfc*)context;
    if(index == NfcMessageTypeDetect) {
        view_dispatcher_switch_to_view(nfc->nfc_common.view_dispatcher, NfcViewDetect);
    }
}

void nfc_view_dispatcher_callback(uint32_t event, void* context) {
    furi_assert(context);

    Nfc* nfc = (Nfc*)context;
    NfcMessage message;
    osMessageQueueGet(nfc->nfc_common.message_queue, &message, NULL, osWaitForever);
    if(event == NfcEventDetect) {
        nfc_detect_view_dispatcher_callback(nfc->nfc_detect, (NfcDetectModel*)&message);
    }
}

Nfc* nfc_alloc() {
    Nfc* nfc = furi_alloc(sizeof(Nfc));

    nfc->nfc_common.message_queue = osMessageQueueNew(8, sizeof(NfcMessage), NULL);
    nfc->nfc_common.worker = nfc_worker_alloc(nfc->nfc_common.message_queue);
    nfc->nfc_common.view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(nfc->nfc_common.view_dispatcher);

    // Open GUI record
    nfc->gui = furi_record_open("gui");
    view_dispatcher_attach_to_gui(
        nfc->nfc_common.view_dispatcher, nfc->gui, ViewDispatcherTypeFullscreen);

    // Menu
    nfc->submenu = submenu_alloc();
    submenu_add_item(nfc->submenu, "Detect", NfcMessageTypeDetect, nfc_menu_callback, nfc);

    View* submenu_view = submenu_get_view(nfc->submenu);
    view_set_previous_callback(submenu_view, nfc_view_exit);
    view_dispatcher_add_view(nfc->nfc_common.view_dispatcher, NfcViewMenu, submenu_view);

    // Detect
    nfc->nfc_detect = nfc_detect_alloc(&nfc->nfc_common);
    view_dispatcher_add_view(
        nfc->nfc_common.view_dispatcher, NfcViewDetect, nfc_detect_get_view(nfc->nfc_detect));

    // Set View Dispatcher custom event callback
    view_dispatcher_set_custom_callback(
        nfc->nfc_common.view_dispatcher, nfc_view_dispatcher_callback, nfc);

    // Switch to menu
    view_dispatcher_switch_to_view(nfc->nfc_common.view_dispatcher, NfcViewMenu);

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

    // Worker
    nfc_worker_stop(nfc->nfc_common.worker);
    nfc_worker_free(nfc->nfc_common.worker);

    // View dispatcher
    view_dispatcher_free(nfc->nfc_common.view_dispatcher);

    // GUI
    furi_record_close("gui");
    nfc->gui = NULL;

    // The rest
    free(nfc);
}

int32_t nfc_task(void* p) {
    Nfc* nfc = nfc_alloc();

    view_dispatcher_run(nfc->nfc_common.view_dispatcher);

    nfc_free(nfc);

    return 0;
}
