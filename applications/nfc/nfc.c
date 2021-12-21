#include "nfc_i.h"
#include "furi-hal-nfc.h"

Nfc* nfc_alloc() {
    Nfc* nfc = furi_alloc(sizeof(Nfc));

    nfc->worker = nfc_worker_alloc();
    nfc->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_allocate_scene_manager(nfc->view_dispatcher, &nfc_scene_handlers, nfc);
    nfc->scene_manager = view_dispatcher_get_scene_manager(nfc->view_dispatcher);
    view_dispatcher_enable_queue(nfc->view_dispatcher);
    view_dispatcher_set_tick_event_period(nfc->view_dispatcher, 100);

    // Nfc device
    nfc->dev = nfc_device_alloc();

    // Open GUI record
    nfc->gui = furi_record_open("gui");
    view_dispatcher_attach_to_gui(nfc->view_dispatcher, nfc->gui, ViewDispatcherTypeFullscreen);

    // Open Notification record
    nfc->notifications = furi_record_open("notification");

    // Submenu
    nfc->submenu = submenu_alloc();
    view_dispatcher_add_view(nfc->view_dispatcher, NfcViewMenu, submenu_get_view(nfc->submenu));

    // Dialog
    nfc->dialog_ex = dialog_ex_alloc();
    view_dispatcher_add_view(
        nfc->view_dispatcher, NfcViewDialogEx, dialog_ex_get_view(nfc->dialog_ex));

    // Popup
    nfc->popup = popup_alloc();
    view_dispatcher_add_view(nfc->view_dispatcher, NfcViewPopup, popup_get_view(nfc->popup));

    // Text Input
    nfc->text_input = text_input_alloc();
    view_dispatcher_add_view(
        nfc->view_dispatcher, NfcViewTextInput, text_input_get_view(nfc->text_input));

    // Byte Input
    nfc->byte_input = byte_input_alloc();
    view_dispatcher_add_view(
        nfc->view_dispatcher, NfcViewByteInput, byte_input_get_view(nfc->byte_input));

    // TextBox
    nfc->text_box = text_box_alloc();
    view_dispatcher_add_view(
        nfc->view_dispatcher, NfcViewTextBox, text_box_get_view(nfc->text_box));
    string_init(nfc->text_box_store);

    // Custom Widget
    nfc->widget = widget_alloc();
    view_dispatcher_add_view(nfc->view_dispatcher, NfcViewWidget, widget_get_view(nfc->widget));

    // Bank Card
    nfc->bank_card = bank_card_alloc();
    view_dispatcher_add_view(
        nfc->view_dispatcher, NfcViewBankCard, bank_card_get_view(nfc->bank_card));

    return nfc;
}

void nfc_free(Nfc* nfc) {
    furi_assert(nfc);

    // Nfc device
    nfc_device_free(nfc->dev);

    // Submenu
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewMenu);
    submenu_free(nfc->submenu);

    // DialogEx
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewDialogEx);
    dialog_ex_free(nfc->dialog_ex);

    // Popup
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewPopup);
    popup_free(nfc->popup);

    // TextInput
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewTextInput);
    text_input_free(nfc->text_input);

    // ByteInput
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewByteInput);
    byte_input_free(nfc->byte_input);

    // TextBox
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewTextBox);
    text_box_free(nfc->text_box);
    string_clear(nfc->text_box_store);

    // Custom Widget
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewWidget);
    widget_free(nfc->widget);

    // Bank Card
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewBankCard);
    bank_card_free(nfc->bank_card);

    // Worker
    nfc_worker_stop(nfc->worker);
    nfc_worker_free(nfc->worker);

    // View Dispatcher
    view_dispatcher_free(nfc->view_dispatcher);

    // GUI
    furi_record_close("gui");
    nfc->gui = NULL;

    // Notifications
    furi_record_close("notification");
    nfc->notifications = NULL;

    free(nfc);
}

void nfc_text_store_set(Nfc* nfc, const char* text, ...) {
    va_list args;
    va_start(args, text);

    vsnprintf(nfc->text_store, sizeof(nfc->text_store), text, args);

    va_end(args);
}

void nfc_text_store_clear(Nfc* nfc) {
    memset(nfc->text_store, 0, sizeof(nfc->text_store));
}

int32_t nfc_app(void* p) {
    Nfc* nfc = nfc_alloc();
    char* args = p;

    // Check argument and run corresponding scene
    if((*args != '\0') && nfc_device_load(nfc->dev, p)) {
        if(nfc->dev->format == NfcDeviceSaveFormatMifareUl) {
            view_dispatcher_set_start_scene(nfc->view_dispatcher, NfcSceneEmulateMifareUl);
        } else {
            view_dispatcher_set_start_scene(nfc->view_dispatcher, NfcSceneEmulateUid);
        }
    } else {
        view_dispatcher_set_start_scene(nfc->view_dispatcher, NfcSceneStart);
    }

    view_dispatcher_run(nfc->view_dispatcher);

    nfc_free(nfc);

    return 0;
}
