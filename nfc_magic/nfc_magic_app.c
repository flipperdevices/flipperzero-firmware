#include "nfc_magic_app_i.h"

bool nfc_magic_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    NfcMagicApp* instance = context;

    return scene_manager_handle_custom_event(instance->scene_manager, event);
}

bool nfc_magic_app_back_event_callback(void* context) {
    furi_assert(context);
    NfcMagicApp* instance = context;

    return scene_manager_handle_back_event(instance->scene_manager);
}

void nfc_magic_app_tick_event_callback(void* context) {
    furi_assert(context);
    NfcMagicApp* instance = context;

    scene_manager_handle_tick_event(instance->scene_manager);
}

void nfc_magic_app_show_loading_popup(void* context, bool show) {
    NfcMagicApp* instance = context;

    if(show) {
        // Raise timer priority so that animations can play
        furi_timer_set_thread_priority(FuriTimerThreadPriorityElevated);
        view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewLoading);
    } else {
        // Restore default timer priority
        furi_timer_set_thread_priority(FuriTimerThreadPriorityNormal);
    }
}

NfcMagicApp* nfc_magic_app_alloc() {
    NfcMagicApp* instance = malloc(sizeof(NfcMagicApp));

    instance->view_dispatcher = view_dispatcher_alloc();
    instance->scene_manager = scene_manager_alloc(&nfc_magic_scene_handlers, instance);
    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_set_event_callback_context(instance->view_dispatcher, instance);
    view_dispatcher_set_custom_event_callback(
        instance->view_dispatcher, nfc_magic_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        instance->view_dispatcher, nfc_magic_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        instance->view_dispatcher, nfc_magic_app_tick_event_callback, 100);

    // Nfc device
    instance->source_dev = nfc_device_alloc();
    nfc_device_set_loading_callback(
        instance->source_dev, nfc_magic_app_show_loading_popup, instance);
    instance->file_path = furi_string_alloc_set(NFC_APP_FOLDER);
    instance->file_name = furi_string_alloc();

    // Open GUI record
    instance->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);

    // Open Notification record
    instance->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Open Dialogs
    instance->dialogs = furi_record_open(RECORD_DIALOGS);

    // Open Storage
    instance->storage = furi_record_open(RECORD_STORAGE);

    // Submenu
    instance->submenu = submenu_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcMagicAppViewMenu, submenu_get_view(instance->submenu));

    // Popup
    instance->popup = popup_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcMagicAppViewPopup, popup_get_view(instance->popup));

    // Loading
    instance->loading = loading_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcMagicAppViewLoading, loading_get_view(instance->loading));

    // Text Input
    instance->text_input = text_input_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher,
        NfcMagicAppViewTextInput,
        text_input_get_view(instance->text_input));

    // Byte Input
    instance->byte_input = byte_input_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher,
        NfcMagicAppViewByteInput,
        byte_input_get_view(instance->byte_input));

    // Custom Widget
    instance->widget = widget_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, NfcMagicAppViewWidget, widget_get_view(instance->widget));

    instance->nfc = nfc_alloc();
    instance->scanner = nfc_magic_scanner_alloc(instance->nfc);

    return instance;
}

void nfc_magic_app_free(NfcMagicApp* instance) {
    furi_assert(instance);

    // Nfc device
    nfc_device_free(instance->source_dev);
    furi_string_free(instance->file_name);
    furi_string_free(instance->file_path);

    // Submenu
    view_dispatcher_remove_view(instance->view_dispatcher, NfcMagicAppViewMenu);
    submenu_free(instance->submenu);

    // Popup
    view_dispatcher_remove_view(instance->view_dispatcher, NfcMagicAppViewPopup);
    popup_free(instance->popup);

    // Loading
    view_dispatcher_remove_view(instance->view_dispatcher, NfcMagicAppViewLoading);
    loading_free(instance->loading);

    // Text Input
    view_dispatcher_remove_view(instance->view_dispatcher, NfcMagicAppViewTextInput);
    text_input_free(instance->text_input);

    // Byte Input
    view_dispatcher_remove_view(instance->view_dispatcher, NfcMagicAppViewByteInput);
    byte_input_free(instance->byte_input);

    // Custom Widget
    view_dispatcher_remove_view(instance->view_dispatcher, NfcMagicAppViewWidget);
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

    // Dialogs
    furi_record_close(RECORD_DIALOGS);
    instance->dialogs = NULL;

    // Storage
    furi_record_close(RECORD_STORAGE);
    instance->storage = NULL;

    nfc_magic_scanner_free(instance->scanner);
    nfc_free(instance->nfc);

    free(instance);
}

static const NotificationSequence nfc_magic_sequence_blink_start_cyan = {
    &message_blink_start_10,
    &message_blink_set_color_cyan,
    &message_do_not_reset,
    NULL,
};

static const NotificationSequence nfc_magic_sequence_blink_stop = {
    &message_blink_stop,
    NULL,
};

void nfc_magic_app_blink_start(NfcMagicApp* instance) {
    notification_message(instance->notifications, &nfc_magic_sequence_blink_start_cyan);
}

void nfc_magic_app_blink_stop(NfcMagicApp* instance) {
    notification_message(instance->notifications, &nfc_magic_sequence_blink_stop);
}

static bool nfc_magic_set_shadow_file_path(FuriString* file_path, FuriString* shadow_file_path) {
    furi_assert(file_path);
    furi_assert(shadow_file_path);

    bool shadow_file_path_set = false;
    if(furi_string_end_with(file_path, NFC_APP_SHADOW_EXTENSION)) {
        furi_string_set(shadow_file_path, file_path);
        shadow_file_path_set = true;
    } else if(furi_string_end_with(file_path, NFC_APP_EXTENSION)) {
        size_t path_len = furi_string_size(file_path);
        // Cut .nfc
        furi_string_set_n(shadow_file_path, file_path, 0, path_len - 4);
        furi_string_cat_printf(shadow_file_path, "%s", NFC_APP_SHADOW_EXTENSION);
        shadow_file_path_set = true;
    }

    return shadow_file_path_set;
}

static bool nfc_magic_has_shadow_file_internal(NfcMagicApp* instance, FuriString* path) {
    furi_assert(path);

    bool has_shadow_file = false;
    FuriString* shadow_file_path = furi_string_alloc();
    do {
        if(furi_string_empty(path)) break;
        if(!nfc_magic_set_shadow_file_path(path, shadow_file_path)) break;
        has_shadow_file =
            storage_common_exists(instance->storage, furi_string_get_cstr(shadow_file_path));
    } while(false);

    furi_string_free(shadow_file_path);

    return has_shadow_file;
}

bool nfc_magic_load_file(NfcMagicApp* instance, FuriString* path, bool show_dialog) {
    furi_assert(instance);
    furi_assert(path);
    bool result = false;

    FuriString* load_path = furi_string_alloc();
    if(nfc_magic_has_shadow_file_internal(instance, path)) {
        nfc_magic_set_shadow_file_path(path, load_path);
    } else if(furi_string_end_with(path, NFC_APP_SHADOW_EXTENSION)) {
        size_t path_len = furi_string_size(path);
        furi_string_set_n(load_path, path, 0, path_len - 4);
        furi_string_cat_printf(load_path, "%s", NFC_APP_EXTENSION);
    } else {
        furi_string_set(load_path, path);
    }

    result = nfc_device_load(instance->source_dev, furi_string_get_cstr(load_path));

    if(result) {
        path_extract_filename(load_path, instance->file_name, true);
    }

    if((!result) && (show_dialog)) {
        dialog_message_show_storage_error(instance->dialogs, "Cannot load\nkey file");
    }

    furi_string_free(load_path);

    return result;
}

bool nfc_magic_load_from_file_select(NfcMagicApp* instance) {
    furi_assert(instance);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, NFC_APP_EXTENSION, &I_Nfc_10px);
    browser_options.base_path = NFC_APP_FOLDER;
    browser_options.hide_dot_files = true;

    // Input events and views are managed by file_browser
    bool result = dialog_file_browser_show(
        instance->dialogs, instance->file_path, instance->file_path, &browser_options);

    if(result) {
        result = nfc_magic_load_file(instance, instance->file_path, true);
    }

    return result;
}

int32_t nfc_magic_app(void* p) {
    UNUSED(p);
    NfcMagicApp* instance = nfc_magic_app_alloc();

    scene_manager_next_scene(instance->scene_manager, NfcMagicSceneStart);

    view_dispatcher_run(instance->view_dispatcher);

    nfc_magic_app_free(instance);

    return 0;
}
