#include "../seader_i.h"
#include <dolphin/dolphin.h>

void seader_scene_read_card_success_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    furi_assert(context);
    Seader* seader = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(seader->view_dispatcher, result);
    }
}

void seader_scene_read_card_success_on_enter(void* context) {
    Seader* seader = context;
    SeaderCredential* credential = seader->credential;
    Widget* widget = seader->widget;

    FuriString* credential_str = furi_string_alloc();
    FuriString* type_str = furi_string_alloc();

    dolphin_deed(DolphinDeedNfcReadSuccess);

    // Send notification
    notification_message(seader->notifications, &sequence_success);

    furi_string_set(credential_str, "");
    if(credential->bit_length > 0) {
        furi_string_cat_printf(
            credential_str, "(%d) %016llx", credential->bit_length, credential->credential);

        if(credential->type == SeaderCredentialType14A) {
            furi_string_set(type_str, "14443A");
        } else if(credential->type == SeaderCredentialTypePicopass) {
            furi_string_set(type_str, "Picopass");
        } else {
            furi_string_set(type_str, "");
        }
    } else {
        furi_string_set(type_str, "Read error");
    }

    widget_add_button_element(
        widget, GuiButtonTypeLeft, "Retry", seader_scene_read_card_success_widget_callback, seader);

    widget_add_button_element(
        widget, GuiButtonTypeRight, "More", seader_scene_read_card_success_widget_callback, seader);

    widget_add_string_element(
        widget, 64, 5, AlignCenter, AlignCenter, FontSecondary, furi_string_get_cstr(type_str));
    widget_add_string_element(
        widget,
        64,
        36,
        AlignCenter,
        AlignCenter,
        FontSecondary,
        furi_string_get_cstr(credential_str));

    furi_string_free(credential_str);
    furi_string_free(type_str);

    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewWidget);
}

bool seader_scene_read_card_success_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = scene_manager_previous_scene(seader->scene_manager);
        } else if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(seader->scene_manager, SeaderSceneCardMenu);
            consumed = true;
        }
    }
    return consumed;
}

void seader_scene_read_card_success_on_exit(void* context) {
    Seader* seader = context;

    // Clear view
    widget_reset(seader->widget);
}
