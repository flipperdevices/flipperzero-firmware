#include "../nfc_i.h"

void nfc_scene_device_info_dialog_callback(GuiButtonType result, void* context) {
    Nfc* nfc = (Nfc*)context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
}

void nfc_scene_device_info_on_enter(void* context) {
    Nfc* nfc = (Nfc*)context;

    // Create device name string
    gui_widget_add_string_element(
        nfc->widget, 22, 7, AlignCenter, AlignTop, FontSecondary, nfc->dev.dev_name);
    // Create left button
    gui_widget_add_button_element(
        nfc->widget, GuiButtonTypeLeft, "Back", nfc_scene_device_info_dialog_callback, nfc);
    // Create right button
    gui_widget_add_button_element(
        nfc->widget, GuiButtonTypeRight, "Data", nfc_scene_device_info_dialog_callback, nfc);
    // Create UID string
    gui_widget_add_string_element(
        nfc->widget, 8, 28, AlignCenter, AlignTop, FontSecondary, "UID huli");
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

const bool nfc_scene_device_info_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = (Nfc*)context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            return scene_manager_previous_scene(nfc->scene_manager);
        } else if(event.event == GuiButtonTypeRight) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            return true;
        }
    }
    return false;
}

const void nfc_scene_device_info_on_exit(void* context) {
    Nfc* nfc = (Nfc*)context;

    gui_widget_clear(nfc->widget);
}
