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

    // Open GUI record
    instance->gui = furi_record_open(RECORD_GUI);

    // Open Notification record
    instance->notifications = furi_record_open(RECORD_NOTIFICATION);

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

    // GUI
    furi_record_close(RECORD_GUI);
    instance->gui = NULL;

    // Notifications
    furi_record_close(RECORD_NOTIFICATION);
    instance->notifications = NULL;

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

bool nfc_save_file(NfcApp* nfc) {
    furi_assert(nfc);
    return true;
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
