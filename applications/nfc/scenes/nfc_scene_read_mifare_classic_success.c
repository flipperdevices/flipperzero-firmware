#include "../nfc_i.h"
#include <dolphin/dolphin.h>

void nfc_scene_read_mifare_classic_success_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    furi_assert(context);
    Nfc* nfc = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

void nfc_scene_read_mifare_classic_success_on_enter(void* context) {
    Nfc* nfc = context;

    DOLPHIN_DEED(DolphinDeedNfcReadSuccess);

    // Send notification
    notification_message(nfc->notifications, &sequence_success);

    // Setup view
    Widget* widget = nfc->widget;
    widget_add_button_element(
        widget,
        GuiButtonTypeLeft,
        "Retry",
        nfc_scene_read_mifare_classic_success_widget_callback,
        nfc);
    widget_add_button_element(
        widget,
        GuiButtonTypeRight,
        "More",
        nfc_scene_read_mifare_classic_success_widget_callback,
        nfc);

    if(string_size(nfc->dev->dev_data.parsed_data)) {
        widget_add_text_box_element(
            nfc->widget,
            0,
            0,
            128,
            32,
            AlignLeft,
            AlignTop,
            string_get_cstr(nfc->dev->dev_data.parsed_data),
            true);
    }

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_read_mifare_classic_success_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = scene_manager_previous_scene(nfc->scene_manager);
        } else if(event.event == GuiButtonTypeRight) {
            // Clear device name
            nfc_device_set_name(nfc->dev, "");
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMifareClassicMenu);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_read_mifare_classic_success_on_exit(void* context) {
    Nfc* nfc = context;

    // Clear view
    widget_reset(nfc->widget);
}
