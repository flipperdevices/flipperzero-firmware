#include "../nfc_app_i.h"

static void nfc_scene_slix_unlock_success_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    NfcApp* instance = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, result);
    }
}

void nfc_scene_slix_unlock_success_on_enter(void* context) {
    NfcApp* instance = context;

    Widget* widget = instance->widget;
    widget_add_string_element(
        widget, 64, 32, AlignCenter, AlignCenter, FontSecondary, "Unlock success");
    widget_add_button_element(
        widget,
        GuiButtonTypeLeft,
        "Retry",
        nfc_scene_slix_unlock_success_widget_callback,
        instance);
    widget_add_button_element(
        widget,
        GuiButtonTypeRight,
        "More",
        nfc_scene_slix_unlock_success_widget_callback,
        instance);

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_slix_unlock_success_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneRetryConfirm);
            consumed = true;
        } else if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(instance->scene_manager, NfcSceneSavedMenu);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        scene_manager_next_scene(instance->scene_manager, NfcSceneExitConfirm);
        consumed = true;
    }

    return consumed;
}

void nfc_scene_slix_unlock_success_on_exit(void* context) {
    NfcApp* instance = context;

    widget_reset(instance->widget);
}
