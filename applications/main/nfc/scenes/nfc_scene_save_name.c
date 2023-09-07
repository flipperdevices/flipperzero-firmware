#include "../nfc_app_i.h"
#include <toolbox/name_generator.h>

void nfc_scene_save_name_text_input_callback(void* context) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcCustomEventTextInputDone);
}

void nfc_scene_save_name_on_enter(void* context) {
    NfcApp* nfc = context;
    FuriString* folder_path = furi_string_alloc();
    TextInput* text_input = nfc->text_input;

    bool name_is_empty = furi_string_empty(nfc->file_name);
    if(name_is_empty) {
        furi_string_set(nfc->file_path, NFC_APP_FOLDER);
        name_generator_make_auto(nfc->text_store, NFC_TEXT_STORE_SIZE, NFC_APP_FILENAME_PREFIX);
        furi_string_set(folder_path, NFC_APP_FOLDER);
    } else {
        nfc_text_store_set(nfc, "%s", furi_string_get_cstr(nfc->file_name));
        path_extract_dirname(furi_string_get_cstr(nfc->file_path), folder_path);
    }

    text_input_set_header_text(text_input, "Name the card");
    text_input_set_result_callback(
        text_input,
        nfc_scene_save_name_text_input_callback,
        nfc,
        nfc->text_store,
        NFC_NAME_SIZE,
        name_is_empty);

    ValidatorIsFile* validator_is_file = validator_is_file_alloc_init(
        furi_string_get_cstr(folder_path),
        NFC_APP_EXTENSION,
        furi_string_get_cstr(nfc->file_name));
    text_input_set_validator(text_input, validator_is_file_callback, validator_is_file);

    furi_string_free(folder_path);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewTextInput);
}

bool nfc_scene_save_name_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventTextInputDone) {
            if(!furi_string_empty(nfc->file_name)) {
                nfc_delete(nfc);
            }
            furi_string_set(nfc->file_name, nfc->text_store);

            if(nfc_save(nfc)) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneSaveSuccess);
                if(scene_manager_has_previous_scene(nfc->scene_manager, NfcSceneSetType)) {
                    dolphin_deed(DolphinDeedNfcAddSave);
                } else {
                    dolphin_deed(DolphinDeedNfcSave);
                }
                consumed = true;
            } else {
                consumed = scene_manager_search_and_switch_to_previous_scene(
                    nfc->scene_manager, NfcSceneStart);
            }
        }
    }

    return consumed;
}

void nfc_scene_save_name_on_exit(void* context) {
    NfcApp* nfc = context;

    // Clear view
    void* validator_context = text_input_get_validator_callback_context(nfc->text_input);
    text_input_set_validator(nfc->text_input, NULL, NULL);
    validator_is_file_free(validator_context);

    text_input_reset(nfc->text_input);
}
