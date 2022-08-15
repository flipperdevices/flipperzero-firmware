#include "../nfc_i.h"


void cb(GuiButtonType result, InputType type, void* context) {
    UNUSED(result);
    UNUSED(type);
    UNUSED(context);
}

void nfc_scene_mf_ultralight_info_on_enter(void* context) {
    Nfc* nfc = context;

    Widget* widget = nfc->widget;
    widget_add_text_scroll_element(
        widget, 0, 0, 128, 52, "\ecWwwwwwwwwwwwwwwwwwwwwwwwww\n\e#Mifare Ultralight\nISO 14443-3(NFC-A)\nUID: 01 02 03 04\nATQA: 00 44 SAK: 00\nPages Read 14/16");
    // widget_add_text_scroll_element(
    //     widget, 0, 0, 128, 52, "\ecWwwwwwwwwwwwwwwwwwwwwwwwww\n\e#Mifare Ultralight\nCheto esho");
    widget_add_button_element(widget, GuiButtonTypeRight, "More", cb, nfc);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_mf_ultralight_info_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);

    return false;
}

void nfc_scene_mf_ultralight_info_on_exit(void* context) {
    Nfc* nfc = context;

    widget_reset(nfc->widget);
}