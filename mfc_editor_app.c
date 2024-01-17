#include "mfc_editor_app_i.h"

bool mfc_editor_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    MfcEditorApp* instance = context;
    return scene_manager_handle_custom_event(instance->scene_manager, event);
}

bool mfc_editor_app_back_event_callback(void* context) {
    furi_assert(context);
    MfcEditorApp* instance = context;
    return scene_manager_handle_back_event(instance->scene_manager);
}

void mfc_editor_app_tick_event_callback(void* context) {
    furi_assert(context);
    MfcEditorApp* instance = context;
    scene_manager_handle_tick_event(instance->scene_manager);
}

MfcEditorApp* mfc_editor_app_alloc() {
    MfcEditorApp* instance = malloc(sizeof(MfcEditorApp));

    instance->view_dispatcher = view_dispatcher_alloc();
    instance->scene_manager = scene_manager_alloc(&mfc_editor_scene_handlers, instance);
    view_dispatcher_enable_queue(instance->view_dispatcher);
    view_dispatcher_set_event_callback_context(instance->view_dispatcher, instance);
    view_dispatcher_set_custom_event_callback(
        instance->view_dispatcher, mfc_editor_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        instance->view_dispatcher, mfc_editor_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        instance->view_dispatcher, mfc_editor_app_tick_event_callback, 100);

    instance->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        instance->view_dispatcher, instance->gui, ViewDispatcherTypeFullscreen);

    instance->storage = furi_record_open(RECORD_STORAGE);

    instance->dialogs = furi_record_open(RECORD_DIALOGS);

    instance->nfc_device = nfc_device_alloc();
    instance->file_path = furi_string_alloc_set(NFC_APP_FOLDER);

    instance->data_view_header = furi_string_alloc();
    instance->data_view_text = furi_string_alloc();

    instance->submenu = submenu_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, MfcEditorAppViewSubmenu, submenu_get_view(instance->submenu));

    instance->popup = popup_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher, MfcEditorAppViewPopup, popup_get_view(instance->popup));

    instance->dialog_ex = dialog_ex_alloc();
    view_dispatcher_add_view(
        instance->view_dispatcher,
        MfcEditorAppViewDialogEx,
        dialog_ex_get_view(instance->dialog_ex));

    return instance;
}

void mfc_editor_app_free(MfcEditorApp* instance) {
    furi_assert(instance);

    view_dispatcher_remove_view(instance->view_dispatcher, MfcEditorAppViewSubmenu);
    submenu_free(instance->submenu);

    view_dispatcher_remove_view(instance->view_dispatcher, MfcEditorAppViewPopup);
    popup_free(instance->popup);

    view_dispatcher_remove_view(instance->view_dispatcher, MfcEditorAppViewDialogEx);
    dialog_ex_free(instance->dialog_ex);

    view_dispatcher_free(instance->view_dispatcher);
    scene_manager_free(instance->scene_manager);

    furi_record_close(RECORD_GUI);
    instance->gui = NULL;

    furi_record_close(RECORD_STORAGE);
    instance->storage = NULL;

    furi_record_close(RECORD_DIALOGS);
    instance->dialogs = NULL;

    nfc_device_free(instance->nfc_device);
    furi_string_free(instance->file_path);

    furi_string_free(instance->data_view_header);
    furi_string_free(instance->data_view_text);

    free(instance);
}

MfcEditorPromptResponse mfc_editor_load_file(MfcEditorApp* instance, FuriString* file_path) {
    furi_assert(instance);
    furi_assert(file_path);

    MfcEditorPromptResponse result = MfcEditorPromptResponseSuccess;

    if(!nfc_device_load(instance->nfc_device, furi_string_get_cstr(file_path))) {
        result = MfcEditorPromptResponseFailure;
        dialog_message_show_storage_error(instance->dialogs, "Cannot load\nkey file");
    } else {
        if(nfc_device_get_protocol(instance->nfc_device) == NfcProtocolMfClassic) {
            instance->mf_classic_data =
                nfc_device_get_data(instance->nfc_device, NfcProtocolMfClassic);
        } else {
            result = MfcEditorPromptResponseNotMfClassic;
        }
    }

    return result;
}

static DialogMessageButton mfc_editor_prompt_should_load_shadow(MfcEditorApp* instance) {
    DialogMessage* message = dialog_message_alloc();
    dialog_message_set_header(message, "File has modifications", 63, 0, AlignCenter, AlignTop);
    dialog_message_set_text(
        message,
        "Would you like to load the\nmodified file (recommended)\nor the original file?",
        63,
        30,
        AlignCenter,
        AlignCenter);
    dialog_message_set_buttons(message, "Original", NULL, "Modified");

    DialogMessageButton message_button = dialog_message_show(instance->dialogs, message);

    dialog_message_free(message);

    return message_button;
}

static void mfc_editor_get_shadow_file_path(FuriString* file_path, FuriString* shadow_file_path) {
    furi_assert(file_path);
    furi_assert(shadow_file_path);

    // Remove NFC extension from end of string then append shadow extension
    furi_string_set_n(shadow_file_path, file_path, 0, furi_string_size(file_path) - 4);
    furi_string_cat_printf(shadow_file_path, "%s", NFC_APP_SHADOW_EXTENSION);
}

static bool mfc_editor_file_has_shadow_file(MfcEditorApp* instance, FuriString* file_path) {
    furi_assert(instance);
    furi_assert(file_path);

    FuriString* shadow_file_path = furi_string_alloc();
    mfc_editor_get_shadow_file_path(file_path, shadow_file_path);
    bool has_shadow_file =
        storage_common_exists(instance->storage, furi_string_get_cstr(shadow_file_path));

    furi_string_free(shadow_file_path);

    return has_shadow_file;
}

MfcEditorPromptResponse mfc_editor_prompt_load_file(MfcEditorApp* instance) {
    furi_assert(instance);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, NFC_APP_EXTENSION, &I_Nfc_10px);
    browser_options.base_path = NFC_APP_FOLDER;
    browser_options.hide_dot_files = true;

    MfcEditorPromptResponse result = MfcEditorPromptResponseSuccess;
    if(!dialog_file_browser_show(
           instance->dialogs, instance->file_path, instance->file_path, &browser_options)) {
        result = MfcEditorPromptResponseExitedFile;
    } else {
        if(mfc_editor_file_has_shadow_file(instance, instance->file_path)) {
            DialogMessageButton message_button = mfc_editor_prompt_should_load_shadow(instance);

            if(message_button == DialogMessageButtonRight) {
                // User selected to use shadow file, so replace selected path with that path
                FuriString* shadow_file_path = furi_string_alloc();
                mfc_editor_get_shadow_file_path(instance->file_path, shadow_file_path);
                furi_string_set(instance->file_path, shadow_file_path);
                furi_string_free(shadow_file_path);
            } else if(message_button == DialogMessageButtonBack) {
                result = MfcEditorPromptResponseExitedShadow;
            }
        }

        // Don't load the file if user was prompted for shadow file use but went back
        if(result == MfcEditorPromptResponseSuccess) {
            result = mfc_editor_load_file(instance, instance->file_path);
        }
    }

    return result;
}

int32_t mfc_editor_app(void* p) {
    UNUSED(p);

    MfcEditorApp* instance = mfc_editor_app_alloc();

    scene_manager_next_scene(instance->scene_manager, MfcEditorSceneStart);

    view_dispatcher_run(instance->view_dispatcher);

    mfc_editor_app_free(instance);

    return 0;
}
