#include "../seader_i.h"
#include <dolphin/dolphin.h>

#define TAG "SeaderCredentialInfoScene"

void seader_scene_credential_info_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    Seader* seader = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(seader->view_dispatcher, result);
    }
}

void seader_scene_credential_info_on_enter(void* context) {
    Seader* seader = context;
    SeaderCredential* credential = seader->credential;
    Widget* widget = seader->widget;

    FuriString* type_str = furi_string_alloc();
    FuriString* bitlength_str = furi_string_alloc();
    FuriString* credential_str = furi_string_alloc();
    FuriString* sio_str = furi_string_alloc();

    dolphin_deed(DolphinDeedNfcReadSuccess);

    // Send notification
    notification_message(seader->notifications, &sequence_success);

    furi_string_set(credential_str, "");
    furi_string_set(bitlength_str, "");
    furi_string_set(sio_str, "");
    if(credential->bit_length > 0) {
        furi_string_cat_printf(bitlength_str, "%d bit", credential->bit_length);
        furi_string_cat_printf(credential_str, "0x%llX", credential->credential);

        if(credential->type == SeaderCredentialTypeNone) {
            furi_string_set(type_str, "None");
        } else if(credential->type == SeaderCredentialType14A) {
            furi_string_set(type_str, "14443A");
        } else if(credential->type == SeaderCredentialTypePicopass) {
            furi_string_set(type_str, "Picopass");
        } else {
            furi_string_set(type_str, "");
        }
    }

    widget_add_button_element(
        seader->widget,
        GuiButtonTypeLeft,
        "Back",
        seader_scene_credential_info_widget_callback,
        seader);

    widget_add_string_element(
        widget, 64, 5, AlignCenter, AlignCenter, FontPrimary, furi_string_get_cstr(type_str));
    widget_add_string_element(
        widget,
        64,
        24,
        AlignCenter,
        AlignCenter,
        FontSecondary,
        furi_string_get_cstr(bitlength_str));
    widget_add_string_element(
        widget,
        64,
        36,
        AlignCenter,
        AlignCenter,
        FontSecondary,
        furi_string_get_cstr(credential_str));

    if(credential->sio[0] == 0x30) {
        furi_string_set(sio_str, "+SIO");
        widget_add_string_element(
            widget, 64, 48, AlignCenter, AlignCenter, FontSecondary, furi_string_get_cstr(sio_str));
    }

    furi_string_free(bitlength_str);
    furi_string_free(type_str);
    furi_string_free(credential_str);
    furi_string_free(sio_str);

    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewWidget);
}

bool seader_scene_credential_info_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = scene_manager_previous_scene(seader->scene_manager);
        } else if(event.event == SeaderCustomEventViewExit) {
            view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewWidget);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewWidget);
        consumed = true;
    }
    return consumed;
}

void seader_scene_credential_info_on_exit(void* context) {
    Seader* seader = context;

    // Clear views
    widget_reset(seader->widget);
}
