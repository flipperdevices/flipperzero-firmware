#include "nfc_app_i.h"

#include <dolphin/dolphin.h>

bool nfc_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    NfcApp* nfc = context;
    return scene_manager_handle_custom_event(nfc->scene_manager, event);
}

bool nfc_back_event_callback(void* context) {
    furi_assert(context);
    NfcApp* nfc = context;
    return scene_manager_handle_back_event(nfc->scene_manager);
}

static void nfc_rpc_command_callback(RpcAppSystemEvent event, void* context) {
    furi_assert(context);
    NfcApp* nfc = context;

    furi_assert(nfc->rpc_ctx);

    if(event == RpcAppEventSessionClose) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcCustomEventRpcSessionClose);
        rpc_system_app_set_callback(nfc->rpc_ctx, NULL, NULL);
        nfc->rpc_ctx = NULL;
    } else if(event == RpcAppEventAppExit) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcCustomEventViewExit);
    } else if(event == RpcAppEventLoadFile) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcCustomEventRpcLoad);
    } else {
        rpc_system_app_confirm(nfc->rpc_ctx, event, false);
    }
}

NfcApp* nfc_app_alloc() {
    NfcApp* instance = malloc(sizeof(NfcApp));

    instance->view_dispatcher = view_dispatcher_alloc();
    instance->scene_manager = scene_manager_alloc(&nfc_scene_handlers, instance);
    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_set_event_callback_context(instance->view_dispatcher, instance);
    view_dispatcher_set_custom_event_callback(
        instance->view_dispatcher, nfc_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        instance->view_dispatcher, nfc_back_event_callback);

    instance->nfc = nfc_alloc();
    instance->nfca_poller = nfca_poller_alloc(instance->nfc);
    instance->mf_ul_poller = mf_ultralight_poller_alloc(instance->nfca_poller);
    instance->mf_desfire_poller = mf_desfire_poller_alloc(instance->nfca_poller);
    instance->nfca_listener = nfca_listener_alloc(instance->nfc);
    instance->mf_ul_listener = mf_ultralight_listener_alloc(instance->nfca_listener);
    instance->nfcb_poller = nfcb_poller_alloc(instance->nfc);

    NfcPollerCollection collection = {
        .nfc = instance->nfc,
        .nfca_poller = instance->nfca_poller,
        .nfcb_poller = instance->nfcb_poller,
    };
    instance->nfc_poller = nfc_poller_alloc(&collection);

    instance->mf_ul_auth = mf_ultralight_auth_alloc();

    // Nfc device
    instance->nfc_dev = nfc_dev_alloc();
    nfc_dev_set_loading_callback(instance->nfc_dev, nfc_show_loading_popup, instance);

    // Open GUI record
    instance->gui = furi_record_open(RECORD_GUI);

    // Open Notification record
    instance->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Open Storage record
    instance->storage = furi_record_open(RECORD_STORAGE);

    // Open Dialogs record
    instance->dialogs = furi_record_open(RECORD_DIALOGS);

    // Submenu
    instance->submenu = submenu_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcViewMenu, submenu_get_view(instance->submenu));

    // Dialog
    instance->dialog_ex = dialog_ex_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcViewDialogEx, dialog_ex_get_view(instance->dialog_ex));

    // Popup
    instance->popup = popup_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcViewPopup, popup_get_view(instance->popup));

    // Loading
    instance->loading = loading_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcViewLoading, loading_get_view(instance->loading));

    // Text Input
    instance->text_input = text_input_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcViewTextInput, text_input_get_view(instance->text_input));

    // Byte Input
    instance->byte_input = byte_input_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcViewByteInput, byte_input_get_view(instance->byte_input));

    // TextBox
    instance->text_box = text_box_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcViewTextBox, text_box_get_view(instance->text_box));
    instance->text_box_store = furi_string_alloc();

    // Custom Widget
    instance->widget = widget_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcViewWidget, widget_get_view(instance->widget));

    instance->file_path = furi_string_alloc_set(NFC_APP_FOLDER);
    instance->file_name = furi_string_alloc();

    return instance;
}

void nfc_app_free(NfcApp* instance) {
    furi_assert(instance);

    // if(instance->rpc_state == NfcRpcStateEmulating) {
    //     // Stop worker
    //     nfc_worker_stop(instance->worker);
    // } else if(instance->rpc_state == NfcRpcStateEmulated) {
    //     // Stop worker
    //     nfc_worker_stop(instance->worker);
    //     // Save data in shadow file
    //     if(furi_string_size(instance->dev->load_path)) {
    //         nfc_device_save_shadow(instance->dev, furi_string_get_cstr(instance->dev->load_path));
    //     }
    // }
    if(instance->rpc_ctx) {
        rpc_system_app_send_exited(instance->rpc_ctx);
        rpc_system_app_set_callback(instance->rpc_ctx, NULL, NULL);
        instance->rpc_ctx = NULL;
    }

    mf_desfire_poller_free(instance->mf_desfire_poller);
    mf_ultralight_listener_free(instance->mf_ul_listener);
    mf_ultralight_poller_free(instance->mf_ul_poller);
    nfca_listener_free(instance->nfca_listener);
    nfca_poller_free(instance->nfca_poller);
    nfcb_poller_free(instance->nfcb_poller);
    nfc_poller_free(instance->nfc_poller);
    nfc_free(instance->nfc);

    mf_ultralight_auth_free(instance->mf_ul_auth);

    // Nfc device
    nfc_dev_free(instance->nfc_dev);

    // Submenu
    view_dispatcher_remove_view(instance->view_dispatcher, NfcViewMenu);
    submenu_free(instance->submenu);

    // DialogEx
    view_dispatcher_remove_view(instance->view_dispatcher, NfcViewDialogEx);
    dialog_ex_free(instance->dialog_ex);

    // Popup
    view_dispatcher_remove_view(instance->view_dispatcher, NfcViewPopup);
    popup_free(instance->popup);

    // Loading
    view_dispatcher_remove_view(instance->view_dispatcher, NfcViewLoading);
    loading_free(instance->loading);

    // TextInput
    view_dispatcher_remove_view(instance->view_dispatcher, NfcViewTextInput);
    text_input_free(instance->text_input);

    // ByteInput
    view_dispatcher_remove_view(instance->view_dispatcher, NfcViewByteInput);
    byte_input_free(instance->byte_input);

    // TextBox
    view_dispatcher_remove_view(instance->view_dispatcher, NfcViewTextBox);
    text_box_free(instance->text_box);
    furi_string_free(instance->text_box_store);

    // Custom Widget
    view_dispatcher_remove_view(instance->view_dispatcher, NfcViewWidget);
    widget_free(instance->widget);

    // View Dispatcher
    view_dispatcher_free(instance->view_dispatcher);

    // Scene Manager
    scene_manager_free(instance->scene_manager);

    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_NOTIFICATION);
    // GUI
    furi_record_close(RECORD_GUI);
    instance->gui = NULL;

    instance->notifications = NULL;

    furi_string_free(instance->file_path);
    furi_string_free(instance->file_name);

    free(instance);
}

void nfc_text_store_set(NfcApp* nfc, const char* text, ...) {
    va_list args;
    va_start(args, text);

    vsnprintf(nfc->text_store, sizeof(nfc->text_store), text, args);

    va_end(args);
}

void nfc_text_store_clear(NfcApp* nfc) {
    memset(nfc->text_store, 0, sizeof(nfc->text_store));
}

void nfc_blink_read_start(NfcApp* nfc) {
    notification_message(nfc->notifications, &sequence_blink_start_cyan);
}

void nfc_blink_emulate_start(NfcApp* nfc) {
    notification_message(nfc->notifications, &sequence_blink_start_magenta);
}

void nfc_blink_detect_start(NfcApp* nfc) {
    notification_message(nfc->notifications, &sequence_blink_start_yellow);
}

void nfc_blink_stop(NfcApp* nfc) {
    notification_message(nfc->notifications, &sequence_blink_stop);
}

void nfc_make_app_folder(NfcApp* instance) {
    furi_assert(instance);

    if(!storage_simply_mkdir(instance->storage, NFC_APP_FOLDER)) {
        dialog_message_show_storage_error(instance->dialogs, "Cannot create\napp folder");
    }
}

bool nfc_save_file(NfcApp* instance, FuriString* path) {
    furi_assert(instance);
    furi_assert(path);

    bool result = nfc_dev_save(
        instance->nfc_dev, &instance->nfc_dev_data, furi_string_get_cstr(instance->file_path));

    if(!result) {
        dialog_message_show_storage_error(instance->dialogs, "Cannot save\nkey file");
    }

    return result;
}

static bool nfc_set_shadow_file_path(FuriString* file_path, FuriString* shadow_file_path) {
    furi_assert(file_path);
    furi_assert(shadow_file_path);

    bool shadow_file_path_set = false;
    if(furi_string_end_with(file_path, NFC_APP_EXTENSION)) {
        size_t path_len = furi_string_size(file_path);
        // Cut .nfc
        furi_string_set_n(shadow_file_path, file_path, 0, path_len - 4);
        furi_string_cat_printf(shadow_file_path, "%s", NFC_APP_SHADOW_EXTENSION);
        shadow_file_path_set = true;
    }

    return shadow_file_path_set;
}

static bool nfc_has_shadow_file_internal(NfcApp* instance, FuriString* path) {
    furi_assert(path);

    bool has_shadow_file = false;
    FuriString* shadow_file_path = furi_string_alloc();
    do {
        if(furi_string_empty(path)) break;
        if(!nfc_set_shadow_file_path(path, shadow_file_path)) break;
        has_shadow_file =
            storage_common_exists(instance->storage, furi_string_get_cstr(shadow_file_path));
    } while(false);

    furi_string_free(shadow_file_path);

    return has_shadow_file;
}

bool nfc_has_shadow_file(NfcApp* instance) {
    furi_assert(instance);

    return nfc_has_shadow_file_internal(instance, instance->file_path);
}

static bool nfc_save_internal(NfcApp* instance, const char* extension) {
    furi_assert(instance);
    furi_assert(extension);

    bool result = false;

    nfc_make_app_folder(instance);

    if(furi_string_end_with(instance->file_path, NFC_APP_EXTENSION)) {
        size_t filename_start = furi_string_search_rchar(instance->file_path, '/');
        furi_string_left(instance->file_path, filename_start);
    }

    furi_string_cat_printf(
        instance->file_path, "/%s%s", furi_string_get_cstr(instance->file_name), extension);

    result = nfc_save_file(instance, instance->file_path);

    return result;
}

bool nfc_save_shadow_file(NfcApp* instance) {
    furi_assert(instance);

    return nfc_save_internal(instance, NFC_APP_EXTENSION);
}

bool nfc_save(NfcApp* instance) {
    furi_assert(instance);

    return nfc_save_internal(instance, NFC_APP_SHADOW_EXTENSION);
}

bool nfc_load_file(NfcApp* instance, FuriString* path, bool show_dialog) {
    furi_assert(instance);
    furi_assert(path);
    bool result = false;

    FuriString* load_path = furi_string_alloc();
    if(nfc_has_shadow_file_internal(instance, path)) {
        nfc_set_shadow_file_path(path, load_path);
    } else {
        furi_string_set(load_path, path);
    }

    result =
        nfc_dev_load(instance->nfc_dev, &instance->nfc_dev_data, furi_string_get_cstr(load_path));
    if(result) {
        path_extract_filename(load_path, instance->file_name, true);
    }

    if((!result) && (show_dialog)) {
        dialog_message_show_storage_error(instance->dialogs, "Cannot load\nkey file");
    }

    furi_string_free(load_path);

    return result;
}

bool nfc_delete(NfcApp* instance) {
    furi_assert(instance);

    bool result = false;
    FuriString* shadow_file_path = furi_string_alloc();

    if(nfc_has_shadow_file(instance)) {
        nfc_set_shadow_file_path(instance->file_path, shadow_file_path);
        storage_simply_remove(instance->storage, furi_string_get_cstr(shadow_file_path));
    }

    result = storage_simply_remove(instance->storage, furi_string_get_cstr(instance->file_path));

    furi_string_free(shadow_file_path);

    return result;
}

bool nfc_load_from_file_select(NfcApp* instance) {
    furi_assert(instance);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, NFC_APP_EXTENSION, &I_Nfc_10px);
    browser_options.base_path = NFC_APP_FOLDER;
    browser_options.hide_dot_files = true;

    // Input events and views are managed by file_browser
    bool result = dialog_file_browser_show(
        instance->dialogs, instance->file_path, instance->file_path, &browser_options);

    if(result) {
        result = nfc_load_file(instance, instance->file_path, true);
    }

    return result;
}

void nfc_show_loading_popup(void* context, bool show) {
    NfcApp* nfc = context;
    TaskHandle_t timer_task = xTaskGetHandle(configTIMER_SERVICE_TASK_NAME);

    if(show) {
        // Raise timer priority so that animations can play
        vTaskPrioritySet(timer_task, configMAX_PRIORITIES - 1);
        view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewLoading);
    } else {
        // Restore default timer priority
        vTaskPrioritySet(timer_task, configTIMER_TASK_PRIORITY);
    }
}

static bool nfc_is_hal_ready() {
    if(f_hal_nfc_is_hal_ready() != FHalNfcErrorNone) {
        // No connection to the chip, show an error screen
        DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
        DialogMessage* message = dialog_message_alloc();
        dialog_message_set_text(
            message,
            "Error!\nNFC chip failed to start\n\n\nSend a photo of this to:\nsupport@flipperzero.one",
            0,
            0,
            AlignLeft,
            AlignTop);
        dialog_message_show(dialogs, message);
        dialog_message_free(message);
        furi_record_close(RECORD_DIALOGS);
        return false;
    } else {
        return true;
    }
}

int32_t nfc_app(void* p) {
    if(!nfc_is_hal_ready()) return 0;

    NfcApp* nfc = nfc_app_alloc();
    char* args = p;

    // Check argument and run corresponding scene
    if(args && strlen(args)) {
        // nfc_device_set_loading_callback(nfc->dev, nfc_show_loading_popup, nfc);
        uint32_t rpc_ctx = 0;
        if(sscanf(p, "RPC %lX", &rpc_ctx) == 1) {
            nfc->rpc_ctx = (void*)rpc_ctx;
            rpc_system_app_set_callback(nfc->rpc_ctx, nfc_rpc_command_callback, nfc);
            rpc_system_app_send_started(nfc->rpc_ctx);
            view_dispatcher_attach_to_gui(
                nfc->view_dispatcher, nfc->gui, ViewDispatcherTypeDesktop);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
        } else {
            view_dispatcher_attach_to_gui(
                nfc->view_dispatcher, nfc->gui, ViewDispatcherTypeFullscreen);
            // if(nfc_device_load(nfc->dev, p, true)) {
            //     if(nfc->dev->format == NfcDeviceSaveFormatMifareUl) {
            //         scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightEmulate);
            //         DOLPHIN_DEED(DolphinDeedNfcEmulate);
            //     } else if(nfc->dev->format == NfcDeviceSaveFormatMifareClassic) {
            //         scene_manager_next_scene(nfc->scene_manager, NfcSceneMfClassicEmulate);
            //         DOLPHIN_DEED(DolphinDeedNfcEmulate);
            //     } else if(nfc->dev->format == NfcDeviceSaveFormatBankCard) {
            //         scene_manager_next_scene(nfc->scene_manager, NfcSceneDeviceInfo);
            //     } else {
            //         scene_manager_next_scene(nfc->scene_manager, NfcSceneEmulateUid);
            //         DOLPHIN_DEED(DolphinDeedNfcEmulate);
            //     }
            // } else {
            //     // Exit app
            //     view_dispatcher_stop(nfc->view_dispatcher);
            // }
        }
        // nfc_device_set_loading_callback(nfc->dev, NULL, nfc);
    } else {
        view_dispatcher_attach_to_gui(
            nfc->view_dispatcher, nfc->gui, ViewDispatcherTypeFullscreen);
        scene_manager_next_scene(nfc->scene_manager, NfcSceneStart);
    }

    view_dispatcher_run(nfc->view_dispatcher);

    nfc_app_free(nfc);

    return 0;
}
