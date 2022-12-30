#include "mag_i.h"

#define TAG "Mag"

static bool mag_debug_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    Mag* mag = context;
    return scene_manager_handle_custom_event(mag->scene_manager, event);
}

static bool mag_debug_back_event_callback(void* context) {
    furi_assert(context);
    Mag* mag = context;
    return scene_manager_handle_back_event(mag->scene_manager);
}

static Mag* mag_alloc() {
    Mag* mag = malloc(sizeof(Mag));

    mag->storage = furi_record_open(RECORD_STORAGE);
    mag->dialogs = furi_record_open(RECORD_DIALOGS);

    mag->file_name = furi_string_alloc();
    mag->file_path = furi_string_alloc_set(MAG_APP_FOLDER);

    mag->view_dispatcher = view_dispatcher_alloc();
    mag->scene_manager = scene_manager_alloc(&mag_scene_handlers, mag);
    view_dispatcher_enable_queue(mag->view_dispatcher);
    view_dispatcher_set_event_callback_context(mag->view_dispatcher, mag);
    view_dispatcher_set_custom_event_callback(
        mag->view_dispatcher, mag_debug_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        mag->view_dispatcher, mag_debug_back_event_callback);

    mag->mag_dev = mag_device_alloc();

    // Open GUI record
    mag->gui = furi_record_open(RECORD_GUI);

    // Open Notification record
    mag->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Submenu
    mag->submenu = submenu_alloc();
    view_dispatcher_add_view(mag->view_dispatcher, MagViewSubmenu, submenu_get_view(mag->submenu));

    // Dialog
    mag->dialog_ex = dialog_ex_alloc();
    view_dispatcher_add_view(
        mag->view_dispatcher, MagViewDialogEx, dialog_ex_get_view(mag->dialog_ex));

    // Popup
    mag->popup = popup_alloc();
    view_dispatcher_add_view(mag->view_dispatcher, MagViewPopup, popup_get_view(mag->popup));

    // Loading
    mag->loading = loading_alloc();
    view_dispatcher_add_view(mag->view_dispatcher, MagViewLoading, loading_get_view(mag->loading));

    // Widget
    mag->widget = widget_alloc();
    view_dispatcher_add_view(mag->view_dispatcher, MagViewWidget, widget_get_view(mag->widget));

    // Text Input
    mag->text_input = text_input_alloc();
    view_dispatcher_add_view(
        mag->view_dispatcher, MagViewTextInput, text_input_get_view(mag->text_input));

    return mag;
}

static void mag_free(Mag* mag) {
    furi_assert(mag);

    furi_string_free(mag->file_name);
    furi_string_free(mag->file_path);

    // Mag device
    mag_device_free(mag->mag_dev);
    mag->mag_dev = NULL;

    // Submenu
    view_dispatcher_remove_view(mag->view_dispatcher, MagViewSubmenu);
    submenu_free(mag->submenu);

    // DialogEx
    view_dispatcher_remove_view(mag->view_dispatcher, MagViewDialogEx);
    dialog_ex_free(mag->dialog_ex);

    // Popup
    view_dispatcher_remove_view(mag->view_dispatcher, MagViewPopup);
    popup_free(mag->popup);

    // Loading
    view_dispatcher_remove_view(mag->view_dispatcher, MagViewLoading);
    loading_free(mag->loading);

    // Widget
    view_dispatcher_remove_view(mag->view_dispatcher, MagViewWidget);
    widget_free(mag->widget);

    // TextInput
    view_dispatcher_remove_view(mag->view_dispatcher, MagViewTextInput);
    text_input_free(mag->text_input);

    // View Dispatcher
    view_dispatcher_free(mag->view_dispatcher);

    // Scene Manager
    scene_manager_free(mag->scene_manager);

    // GUI
    furi_record_close(RECORD_GUI);
    mag->gui = NULL;

    // Notifications
    furi_record_close(RECORD_NOTIFICATION);
    mag->notifications = NULL;

    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);

    free(mag);
}

// entry point for app
int32_t mag_app(void* p) {
    FURI_LOG_D(TAG, "Start");
    Mag* mag = mag_alloc();
    FURI_LOG_D(TAG, "Mag alloc-ed");
    UNUSED(p);

    mag_make_app_folder(mag);
    FURI_LOG_D(TAG, "mag_make_app_folder done");

    view_dispatcher_attach_to_gui(mag->view_dispatcher, mag->gui, ViewDispatcherTypeFullscreen);
    FURI_LOG_D(TAG, "view_dispatcher_attach... done");
    scene_manager_next_scene(mag->scene_manager, MagSceneStart);
    FURI_LOG_D(TAG, "scene manager next scene done");

    view_dispatcher_run(mag->view_dispatcher);
    FURI_LOG_D(TAG, "view dispatcher run done");

    mag_free(mag);
    FURI_LOG_D(TAG, "mag free done");

    return 0;
}

bool mag_save_key(Mag* mag) {
    furi_assert(mag);

    bool result = false;

    mag_make_app_folder(mag);

    if(furi_string_end_with(mag->file_path, MAG_APP_EXTENSION)) {
        size_t filename_start = furi_string_search_rchar(mag->file_path, '/');
        furi_string_left(mag->file_path, filename_start);
    }

    furi_string_cat_printf(
        mag->file_path, "/%s%s", furi_string_get_cstr(mag->file_name), MAG_APP_EXTENSION);

    result = mag_save_key_data(mag, mag->file_path);
    return result;
}

bool mag_load_key_from_file_select(Mag* mag) {
    furi_assert(mag);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, MAG_APP_EXTENSION, &I_mag_10px);
    browser_options.base_path = MAG_APP_FOLDER;

    // Input events and views are managed by file_browser
    bool result =
        dialog_file_browser_show(mag->dialogs, mag->file_path, mag->file_path, &browser_options);

    if(result) {
        result = mag_load_key_data(mag, mag->file_path, true);
    }

    return result;
}

bool mag_delete_key(Mag* mag) {
    furi_assert(mag);

    return storage_simply_remove(mag->storage, furi_string_get_cstr(mag->file_path));
}

bool mag_load_key_data(Mag* mag, FuriString* path, bool show_dialog) {
    bool result = false;
    UNUSED(mag);
    UNUSED(path);
    UNUSED(show_dialog);

    // TODO: Needs reworking from LFRFID version, as that goes through some custom protocol by key type.
    //       Alternatively, co-opt the "protocol" typing as our way of encoding track # (Track 1, 2, 3, or some combination thereof)

    return result;
}

bool mag_save_key_data(Mag* mag, FuriString* path) {
    bool result = false;
    UNUSED(path);
    //bool result = lfrfid_dict_file_save(app->dict, app->protocol_id, furi_string_get_cstr(path));
    // TODO: needs reworking from LFRFID version
    if(!result) {
        dialog_message_show_storage_error(mag->dialogs, "Cannot save\nkey file");
    }

    return result;
}

void mag_make_app_folder(Mag* mag) {
    furi_assert(mag);

    if(!storage_simply_mkdir(mag->storage, MAG_APP_FOLDER)) {
        dialog_message_show_storage_error(mag->dialogs, "Cannot create\napp folder");
    }
}

void mag_text_store_set(Mag* mag, const char* text, ...) {
    furi_assert(mag);
    va_list args;
    va_start(args, text);

    vsnprintf(mag->text_store, MAG_TEXT_STORE_SIZE, text, args);

    va_end(args);
}

void mag_text_store_clear(Mag* mag) {
    furi_assert(mag);
    memset(mag->text_store, 0, sizeof(mag->text_store));
}

void mag_popup_timeout_callback(void* context) {
    Mag* mag = context;
    view_dispatcher_send_custom_event(mag->view_dispatcher, MagEventPopupClosed);
}

void mag_widget_callback(GuiButtonType result, InputType type, void* context) {
    Mag* mag = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(mag->view_dispatcher, result);
    }
}

void mag_text_input_callback(void* context) {
    Mag* mag = context;
    view_dispatcher_send_custom_event(mag->view_dispatcher, MagEventNext);
}

void mag_show_loading_popup(void* context, bool show) {
    Mag* mag = context;
    TaskHandle_t timer_task = xTaskGetHandle(configTIMER_SERVICE_TASK_NAME);

    if(show) {
        // Raise timer priority so that animations can play
        vTaskPrioritySet(timer_task, configMAX_PRIORITIES - 1);
        view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewLoading);
    } else {
        // Restore default timer priority
        vTaskPrioritySet(timer_task, configTIMER_TASK_PRIORITY);
    }
}
