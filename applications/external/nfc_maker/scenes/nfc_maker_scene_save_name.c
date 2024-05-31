#include "../nfc_maker.h"
#include <nfc_device_i.h>
#include <core/string.h>

enum TextInputResult {
    TextInputResultOk,
};

void nfc_maker_get_abbreviated_name(const NfcDevice* instance, FuriString* name) {
    furi_assert(instance);
    furi_assert(instance->protocol < NfcProtocolNum);

    furi_string_set(name, nfc_device_get_name(instance, NfcDeviceNameTypeFull));
    furi_string_replace(name, "Mifare", "MF");
    furi_string_replace(name, " Classic", "C"); // MFC
    furi_string_replace(name, "Desfire", "Des"); // MF Des
    furi_string_replace(name, "Ultralight", "UL"); // MF UL
    furi_string_replace(name, " Plus", "+"); // NTAG I2C+
    furi_string_replace(name, " (Unknown)", "");
}

static void nfc_maker_scene_save_name_text_input_callback(void* context) {
    NfcMaker* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, TextInputResultOk);
}

void nfc_maker_scene_save_name_on_enter(void* context) {
    NfcMaker* app = context;
    NFCMaker_TextInput* text_input = app->text_input;

    nfc_maker_text_input_set_header_text(text_input, "Save the NFC tag:");

    FuriString* prefix = furi_string_alloc();
    nfc_maker_get_abbreviated_name(app->nfc_device, prefix);
    furi_string_replace(prefix, "Mifare", "MF");
    furi_string_replace(prefix, " Classic", "C"); // MFC
    furi_string_replace(prefix, "Desfire", "Des"); // MF Des
    furi_string_replace(prefix, "Ultralight", "UL"); // MF UL
    furi_string_replace(prefix, " Plus", "+"); // NTAG I2C+
    furi_string_replace(prefix, " (Unknown)", "");
    furi_string_replace_all(prefix, " ", "_");
    name_generator_make_auto(app->save_buf, BIG_INPUT_LEN, furi_string_get_cstr(prefix));
    furi_string_free(prefix);

    nfc_maker_text_input_set_result_callback(
        text_input,
        nfc_maker_scene_save_name_text_input_callback,
        app,
        app->save_buf,
        BIG_INPUT_LEN,
        true);

    ValidatorIsFile* validator_is_file =
        validator_is_file_alloc_init(NFC_APP_FOLDER, NFC_APP_EXTENSION, NULL);
    nfc_maker_text_input_set_validator(text_input, validator_is_file_callback, validator_is_file);

    view_dispatcher_switch_to_view(app->view_dispatcher, NfcMakerViewTextInput);
}

bool nfc_maker_scene_save_name_on_event(void* context, SceneManagerEvent event) {
    NfcMaker* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case TextInputResultOk:
            scene_manager_next_scene(app->scene_manager, NfcMakerSceneSaveResult);
            break;
        default:
            break;
        }
    }

    return consumed;
}

void nfc_maker_scene_save_name_on_exit(void* context) {
    NfcMaker* app = context;
    nfc_maker_text_input_reset(app->text_input);
}
