#include "nfc_crocodile_i.h"

#define TAG "NfcCrocodileMain"

bool nfc_crocodile_back_event_callback(void* context) {
    furi_assert(context);
    NfcCrocodile* nfc_crocodile = context;
    return scene_manager_handle_back_event(nfc_crocodile->scene_manager);
}

bool nfc_crocodile_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    NfcCrocodile* nfc_crocodile = context;
    return scene_manager_handle_custom_event(nfc_crocodile->scene_manager, event);
}

NfcCrocodile* nfc_crocodile_alloc() {
    NfcCrocodile* context = (NfcCrocodile*)malloc(sizeof(NfcCrocodile));
    furi_assert(context);

    // SM&VD
    context->scene_manager = scene_manager_alloc(&nfc_crocodile_scene_handlers, context);
    context->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(context->view_dispatcher);
    view_dispatcher_set_event_callback_context(context->view_dispatcher, context);
    view_dispatcher_set_navigation_event_callback(
        context->view_dispatcher, nfc_crocodile_back_event_callback);
    view_dispatcher_set_custom_event_callback(
        context->view_dispatcher, nfc_crocodile_custom_event_callback);

    // GUI
    context->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        context->view_dispatcher, context->gui, ViewDispatcherTypeFullscreen);

    // Notification
    //context->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Submenu
    context->submenu = submenu_alloc();
    view_dispatcher_add_view(
        context->view_dispatcher, NfcCrocodileViewMenu, submenu_get_view(context->submenu));

    // Popup
    context->popup = popup_alloc();
    view_dispatcher_add_view(
        context->view_dispatcher, NfcCrocodileViewPopup, popup_get_view(context->popup));

    // Variable Item List
    context->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        context->view_dispatcher,
        NfcCrocodileViewVariableItemList,
        variable_item_list_get_view(context->variable_item_list));

    // Dialog
    context->dialog = dialog_ex_alloc();
    view_dispatcher_add_view(
        context->view_dispatcher, NfcCrocodileViewDialog, dialog_ex_get_view(context->dialog));

    // Dialogs
    //context->dialogs = furi_record_open(RECORD_DIALOGS);

    // Storage
    context->storage = furi_record_open(RECORD_STORAGE);
    storage_simply_mkdir(context->storage, STORAGE_APP_DATA_PATH_PREFIX);

    // Internal
    context->storage_type = NfcCrocodileStorageText;
    context->card_content = NULL;
    context->nfc_crocodile_worker = nfc_crocodile_worker_alloc();

    return context;
}

void nfc_crocodile_free(NfcCrocodile* context) {
    furi_assert(context);

    // Internal
    nfc_crocodile_worker_free(context->nfc_crocodile_worker);
    if(context->card_content) {
        free(context->card_content);
    }

    // Storage
    furi_record_close(RECORD_STORAGE);

    // Submenu
    view_dispatcher_remove_view(context->view_dispatcher, NfcCrocodileViewMenu);

    // Popup
    view_dispatcher_remove_view(context->view_dispatcher, NfcCrocodileViewPopup);

    // Variable Item List
    view_dispatcher_remove_view(context->view_dispatcher, NfcCrocodileViewVariableItemList);

    // Dialog
    view_dispatcher_remove_view(context->view_dispatcher, NfcCrocodileViewDialog);

    // Dialogs
    //furi_record_close(RECORD_DIALOGS);
    //context->dialogs = NULL;

    // SM&VD
    scene_manager_free(context->scene_manager);
    view_dispatcher_free(context->view_dispatcher);
    submenu_free(context->submenu);
    popup_free(context->popup);
    variable_item_list_free(context->variable_item_list);
    dialog_ex_free(context->dialog);

    // GUI
    furi_record_close(RECORD_GUI);
    context->gui = NULL;

    // Notification
    //furi_record_close(RECORD_NOTIFICATION);
    //context->notifications = NULL;

    free(context);
}

int32_t nfc_crocodile_main(void* p) {
    UNUSED(p);
    NfcCrocodile* context = nfc_crocodile_alloc();

    scene_manager_next_scene(context->scene_manager, NfcCrocodileSceneMainMenu);
    view_dispatcher_run(context->view_dispatcher);

    nfc_crocodile_free(context);
    return 0;
}