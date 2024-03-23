#include "../nfc_magic_app_i.h"

#define CONFIG_SIZE (32)

void nfc_magic_scene_gen4_show_cfg_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    NfcMagicApp* instance = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, result);
    }
}

void nfc_magic_scene_gen4_show_cfg_on_enter(void* context) {
    NfcMagicApp* instance = context;
    Widget* widget = instance->widget;

    FuriString* output = furi_string_alloc();

    for(size_t i = 0; i < CONFIG_SIZE; i += 2) {
        if(i && !(i % 8)) furi_string_cat_printf(output, "\n");
        furi_string_cat_printf(
            output, "%02X%02X ", instance->gen4_config[i], instance->gen4_config[i + 1]);
    }

    widget_add_string_element(widget, 3, 4, AlignLeft, AlignTop, FontPrimary, "Gen4 Config");

    widget_add_text_scroll_element(widget, 3, 17, 124, 50, furi_string_get_cstr(output));

    furi_string_free(output);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewWidget);
}

bool nfc_magic_scene_gen4_show_cfg_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            instance->scene_manager, NfcMagicSceneGen4GetInfo);
    }
    return consumed;
}

void nfc_magic_scene_gen4_show_cfg_on_exit(void* context) {
    NfcMagicApp* instance = context;

    widget_reset(instance->widget);
}
