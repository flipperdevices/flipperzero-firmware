#include "nfc_i.h"

uint32_t nfc_view_read_exit(void* context) {
    furi_assert(context);
    Nfc* nfc = context;
    NfcMessage message;
    message.type = NfcMessageTypeWorkerStop;
    furi_check(osMessageQueuePut(nfc->message_queue, &message, 0, osWaitForever) == osOK);
    return VIEW_NONE;
}

Nfc* nfc_alloc() {
    Nfc* nfc = furi_alloc(sizeof(Nfc));

    nfc->message_queue = osMessageQueueNew(8, sizeof(NfcMessage), NULL);
    nfc->worker = nfc_worker_alloc(nfc->message_queue);

    nfc->icon = assets_icons_get(A_NFC_14);
    nfc->menu_vm = furi_open("menu");
    furi_check(nfc->menu_vm);

    nfc->menu = menu_item_alloc_menu("NFC", nfc->icon);
    menu_item_subitem_add(
        nfc->menu, menu_item_alloc_function("Detect", NULL, nfc_menu_detect_callback, nfc));
    menu_item_subitem_add(
        nfc->menu, menu_item_alloc_function("Emulate", NULL, nfc_menu_emulate_callback, nfc));
    menu_item_subitem_add(
        nfc->menu, menu_item_alloc_function("Field On", NULL, nfc_menu_field_on_callback, nfc));
    menu_item_subitem_add(
        nfc->menu, menu_item_alloc_function("Field Off", NULL, nfc_menu_field_off_callback, nfc));

    nfc->view_read = view_alloc();
    view_set_context(nfc->view_read, nfc);
    view_set_draw_callback(nfc->view_read, nfc_view_read_draw);
    view_set_previous_callback(nfc->view_read, nfc_view_read_exit);
    view_allocate_model(nfc->view_read, ViewModelTypeLocking, sizeof(NfcViewReadModel));
    nfc->view_emulate = view_alloc();
    view_set_context(nfc->view_emulate, nfc);
    view_set_draw_callback(nfc->view_emulate, nfc_view_emulate_draw);
    view_set_previous_callback(nfc->view_emulate, nfc_view_read_exit);
    nfc->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_add_view(nfc->view_dispatcher, NfcViewRead, nfc->view_read);
    view_dispatcher_add_view(nfc->view_dispatcher, NfcViewEmulate, nfc->view_emulate);

    return nfc;
}

void nfc_menu_detect_callback(void* context) {
    furi_assert(context);
    Nfc* nfc = context;
    NfcMessage message;
    message.type = NfcMessageTypeDetect;
    furi_check(osMessageQueuePut(nfc->message_queue, &message, 0, osWaitForever) == osOK);
}

void nfc_menu_emulate_callback(void* context) {
    furi_assert(context);
    Nfc* nfc = context;
    NfcMessage message;
    message.type = NfcMessageTypeEmulate;
    furi_check(osMessageQueuePut(nfc->message_queue, &message, 0, osWaitForever) == osOK);
}

void nfc_menu_field_on_callback(void* context) {
    furi_assert(context);
    Nfc* nfc = context;
    NfcMessage message;
    message.type = NfcMessageTypeFieldOn;
    furi_check(osMessageQueuePut(nfc->message_queue, &message, 0, osWaitForever) == osOK);
}

void nfc_menu_field_off_callback(void* context) {
    furi_assert(context);
    Nfc* nfc = context;
    NfcMessage message;
    message.type = NfcMessageTypeFieldOff;
    furi_check(osMessageQueuePut(nfc->message_queue, &message, 0, osWaitForever) == osOK);
}

void nfc_task(void* p) {
    Nfc* nfc = nfc_alloc();

    Gui* gui = furi_open("gui");
    view_dispatcher_attach_to_gui(nfc->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    with_value_mutex(
        nfc->menu_vm, (Menu * menu) { menu_item_add(menu, nfc->menu); });

    if(!furi_create("nfc", nfc)) {
        printf("[nfc_task] cannot create nfc record\n");
        furiac_exit(NULL);
    }

    furiac_ready();

    NfcMessage message;
    while(1) {
        furi_check(osMessageQueueGet(nfc->message_queue, &message, NULL, osWaitForever) == osOK);
        if(message.type == NfcMessageTypeDetect) {
            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewRead);
            nfc_worker_start(nfc->worker, NfcWorkerStatePoll);
        } else if(message.type == NfcMessageTypeEmulate) {
            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewEmulate);
            nfc_worker_start(nfc->worker, NfcWorkerStateEmulate);
        } else if(message.type == NfcMessageTypeWorkerStop) {
            nfc_worker_stop(nfc->worker);
        } else if(message.type == NfcMessageTypeFieldOn) {
            nfc_worker_field_on(nfc->worker);
        } else if(message.type == NfcMessageTypeFieldOff) {
            nfc_worker_field_off(nfc->worker);
        } else if(message.type == NfcMessageTypeDeviceFound) {
            with_view_model(
                nfc->view_read, (NfcViewReadModel * model) {
                    model->status = NfcViewReadModelStatusFound;
                    model->device = message.device;
                });
        } else if(message.type == NfcMessageTypeWorkerStateChange) {
            NfcViewReadModelStatus status;
            if(message.worker_state == NfcWorkerStateBroken) {
                status = NfcViewReadModelStatusError;
            } else if(message.worker_state == NfcWorkerStateReady) {
                status = NfcViewReadModelStatusReady;
            } else if(message.worker_state == NfcWorkerStatePoll) {
                status = NfcViewReadModelStatusSearching;
            }

            with_view_model(
                nfc->view_read, (NfcViewReadModel * model) { model->status = status; });
        }
    }
}
