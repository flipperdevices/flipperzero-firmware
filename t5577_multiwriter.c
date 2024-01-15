#include "core/core_defines.h"
#include "t5577_multiwriter_i.h"
#include <dolphin/dolphin.h>

static bool t5577_multiwriter_debug_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    LfRfid* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool t5577_multiwriter_debug_back_event_callback(void* context) {
    furi_assert(context);
    LfRfid* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static LfRfid* t5577_multiwriter_alloc() {
    LfRfid* t5577_multiwriter = malloc(sizeof(LfRfid));

    t5577_multiwriter->storage = furi_record_open(RECORD_STORAGE);
    t5577_multiwriter->dialogs = furi_record_open(RECORD_DIALOGS);

    t5577_multiwriter->file_name = furi_string_alloc();
    t5577_multiwriter->file_path = furi_string_alloc_set(LFRFID_APP_FOLDER);

    t5577_multiwriter->dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);

    size_t size = protocol_dict_get_max_data_size(t5577_multiwriter->dict);
    t5577_multiwriter->new_key_data = (uint8_t*)malloc(size);
    t5577_multiwriter->old_key_data = (uint8_t*)malloc(size);

    t5577_multiwriter->lfworker = lfrfid_worker_alloc(t5577_multiwriter->dict);

    t5577_multiwriter->view_dispatcher = view_dispatcher_alloc();
    t5577_multiwriter->scene_manager =
        scene_manager_alloc(&t5577_multiwriter_scene_handlers, t5577_multiwriter);
    view_dispatcher_enable_queue(t5577_multiwriter->view_dispatcher);
    view_dispatcher_set_event_callback_context(
        t5577_multiwriter->view_dispatcher, t5577_multiwriter);
    view_dispatcher_set_custom_event_callback(
        t5577_multiwriter->view_dispatcher, t5577_multiwriter_debug_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        t5577_multiwriter->view_dispatcher, t5577_multiwriter_debug_back_event_callback);

    // Open GUI record
    t5577_multiwriter->gui = furi_record_open(RECORD_GUI);

    // Open Notification record
    t5577_multiwriter->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Submenu
    t5577_multiwriter->submenu = submenu_alloc();
    view_dispatcher_add_view(
        t5577_multiwriter->view_dispatcher,
        LfRfidViewSubmenu,
        submenu_get_view(t5577_multiwriter->submenu));

    // Dialog
    t5577_multiwriter->dialog_ex = dialog_ex_alloc();
    view_dispatcher_add_view(
        t5577_multiwriter->view_dispatcher,
        LfRfidViewDialogEx,
        dialog_ex_get_view(t5577_multiwriter->dialog_ex));

    // Popup
    t5577_multiwriter->popup = popup_alloc();
    view_dispatcher_add_view(
        t5577_multiwriter->view_dispatcher,
        LfRfidViewPopup,
        popup_get_view(t5577_multiwriter->popup));

    // Widget
    t5577_multiwriter->widget = widget_alloc();
    view_dispatcher_add_view(
        t5577_multiwriter->view_dispatcher,
        LfRfidViewWidget,
        widget_get_view(t5577_multiwriter->widget));

    // Text Input
    t5577_multiwriter->text_input = text_input_alloc();
    view_dispatcher_add_view(
        t5577_multiwriter->view_dispatcher,
        LfRfidViewTextInput,
        text_input_get_view(t5577_multiwriter->text_input));

    // Byte Input
    t5577_multiwriter->byte_input = byte_input_alloc();
    view_dispatcher_add_view(
        t5577_multiwriter->view_dispatcher,
        LfRfidViewByteInput,
        byte_input_get_view(t5577_multiwriter->byte_input));

    return t5577_multiwriter;
} //-V773

static void t5577_multiwriter_free(LfRfid* t5577_multiwriter) {
    furi_assert(t5577_multiwriter);

    furi_string_free(t5577_multiwriter->file_name);
    furi_string_free(t5577_multiwriter->file_path);
    protocol_dict_free(t5577_multiwriter->dict);

    lfrfid_worker_free(t5577_multiwriter->lfworker);

    free(t5577_multiwriter->new_key_data);
    free(t5577_multiwriter->old_key_data);

    // Submenu
    view_dispatcher_remove_view(t5577_multiwriter->view_dispatcher, LfRfidViewSubmenu);
    submenu_free(t5577_multiwriter->submenu);

    // DialogEx
    view_dispatcher_remove_view(t5577_multiwriter->view_dispatcher, LfRfidViewDialogEx);
    dialog_ex_free(t5577_multiwriter->dialog_ex);

    // Popup
    view_dispatcher_remove_view(t5577_multiwriter->view_dispatcher, LfRfidViewPopup);
    popup_free(t5577_multiwriter->popup);

    // Widget
    view_dispatcher_remove_view(t5577_multiwriter->view_dispatcher, LfRfidViewWidget);
    widget_free(t5577_multiwriter->widget);

    // TextInput
    view_dispatcher_remove_view(t5577_multiwriter->view_dispatcher, LfRfidViewTextInput);
    text_input_free(t5577_multiwriter->text_input);

    // ByteInput
    view_dispatcher_remove_view(t5577_multiwriter->view_dispatcher, LfRfidViewByteInput);
    byte_input_free(t5577_multiwriter->byte_input);

    // View Dispatcher
    view_dispatcher_free(t5577_multiwriter->view_dispatcher);

    // Scene Manager
    scene_manager_free(t5577_multiwriter->scene_manager);

    // GUI
    furi_record_close(RECORD_GUI);
    t5577_multiwriter->gui = NULL;

    // Notifications
    furi_record_close(RECORD_NOTIFICATION);
    t5577_multiwriter->notifications = NULL;

    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_DIALOGS);

    free(t5577_multiwriter);
}

int32_t t5577_multiwriter_app(void* p) {
    LfRfid* app = t5577_multiwriter_alloc();
    UNUSED(p);

    t5577_multiwriter_make_app_folder(app);

    {
        view_dispatcher_attach_to_gui(
            app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
        scene_manager_next_scene(app->scene_manager, LfRfidSceneStart);
    }

    view_dispatcher_run(app->view_dispatcher);

    t5577_multiwriter_free(app);

    return 0;
}

bool t5577_multiwriter_load_key_from_file_select(LfRfid* app) {
    furi_assert(app);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(
        &browser_options, LFRFID_APP_FILENAME_EXTENSION, &I_125_10px);
    browser_options.base_path = LFRFID_APP_FOLDER;

    // Input events and views are managed by file_browser
    bool result =
        dialog_file_browser_show(app->dialogs, app->file_path, app->file_path, &browser_options);

    if(result) {
        result = t5577_multiwriter_load_key_data(app, app->file_path, true);
    }

    return result;
}

bool t5577_multiwriter_load_key_data(LfRfid* app, FuriString* path, bool show_dialog) {
    bool result = false;

    do {
        app->protocol_id = lfrfid_dict_file_load(app->dict, furi_string_get_cstr(path));
        if(app->protocol_id == PROTOCOL_NO) break;
        if(app->protocol_id != LFRFIDProtocolEM4100) break;

        path_extract_filename(path, app->file_name, true);
        result = true;
    } while(0);

    if((!result) && (show_dialog)) {
        dialog_message_show_storage_error(app->dialogs, "Unsupported\nlfrfid protocol!");
    }

    return result;
}

void t5577_multiwriter_make_app_folder(LfRfid* app) {
    furi_assert(app);

    if(!storage_simply_mkdir(app->storage, LFRFID_APP_FOLDER)) {
        dialog_message_show_storage_error(app->dialogs, "Cannot create\napp folder");
    }
}

void t5577_multiwriter_text_store_set(LfRfid* app, const char* text, ...) {
    furi_assert(app);
    va_list args;
    va_start(args, text);

    vsnprintf(app->text_store, LFRFID_TEXT_STORE_SIZE, text, args);

    va_end(args);
}

void t5577_multiwriter_text_store_clear(LfRfid* app) {
    furi_assert(app);
    memset(app->text_store, 0, sizeof(app->text_store));
}

void t5577_multiwriter_popup_timeout_callback(void* context) {
    LfRfid* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, LfRfidEventPopupClosed);
}

void t5577_multiwriter_widget_callback(GuiButtonType result, InputType type, void* context) {
    LfRfid* app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(app->view_dispatcher, result);
    }
}

void t5577_multiwriter_text_input_callback(void* context) {
    LfRfid* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, LfRfidEventNext);
}
