#include "../nfc_i.h"

void nfc_scene_mf_ultralight_info_on_enter(void* context) {
    Nfc* nfc = context;

    Widget* widget = nfc->widget;
    widget_add_text_scroll_element(
        widget, 0, 0, 128, 55, "\ecWwwwwwwwwwwwwwwwwwwwwwwwww\n\e#Mifare Ultralight\nCheto esho");
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