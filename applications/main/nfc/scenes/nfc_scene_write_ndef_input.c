#include "../nfc_app_i.h"

static void nfc_scene_write_ndef_input_callback(void* context) {
    NfcApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventTextInputDone);
}

void nfc_scene_write_ndef_input_on_enter(void* context) {
    NfcApp* instance = context;
    NdefTextInput* text_input = instance->ndef_text_input;

    const char* header = "Enter value";
    const char* seed = NULL;

    if(instance->ndef_write.input_step == NdefWriteInputStepPrimary) {
        switch(instance->ndef_write.record_type) {
        case NdefWriteRecordTypeUri:
            header = "Enter URL";
            seed = "https://";
            break;
        case NdefWriteRecordTypeEmail:
            header = "Enter email address";
            break;
        case NdefWriteRecordTypePhone:
            header = "Enter phone number";
            seed = "+";
            break;
        case NdefWriteRecordTypeText:
            header = "Enter text";
            break;
        case NdefWriteRecordTypeWifi:
            header = "Enter Wi-Fi SSID";
            break;
        }
    } else {
        header = "Enter Wi-Fi password";
    }

    // Seed buffer with prior value (if any) or scheme hint
    memset(instance->text_store, 0, sizeof(instance->text_store));
    const char* prior =
        (instance->ndef_write.input_step == NdefWriteInputStepPrimary) ?
            furi_string_get_cstr(instance->ndef_write.primary) :
            furi_string_get_cstr(instance->ndef_write.secondary);
    if(prior && prior[0] != '\0') {
        strlcpy(instance->text_store, prior, sizeof(instance->text_store));
    } else if(seed) {
        strlcpy(instance->text_store, seed, sizeof(instance->text_store));
    }

    ndef_text_input_set_header_text(text_input, header);
    ndef_text_input_set_result_callback(
        text_input,
        nfc_scene_write_ndef_input_callback,
        instance,
        instance->text_store,
        sizeof(instance->text_store),
        false);

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewNdefTextInput);
}

bool nfc_scene_write_ndef_input_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventTextInputDone) {
            NdefWriteContext* ctx = &instance->ndef_write;
            if(ctx->input_step == NdefWriteInputStepPrimary) {
                furi_string_set(ctx->primary, instance->text_store);
                if(ctx->record_type == NdefWriteRecordTypeWifi) {
                    // Wi-Fi needs a second prompt for the password.
                    ctx->input_step = NdefWriteInputStepSecondary;
                    scene_manager_next_scene(
                        instance->scene_manager, NfcSceneWriteNdefInput);
                } else {
                    scene_manager_next_scene(
                        instance->scene_manager, NfcSceneWriteNdefTag);
                }
            } else {
                furi_string_set(ctx->secondary, instance->text_store);
                scene_manager_next_scene(instance->scene_manager, NfcSceneWriteNdefTag);
            }
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_write_ndef_input_on_exit(void* context) {
    NfcApp* instance = context;
    ndef_text_input_reset(instance->ndef_text_input);
}
