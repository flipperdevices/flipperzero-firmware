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

    notification_message(instance->notifications, &sequence_success);

    FuriString* temp_config = furi_string_alloc();
    for(size_t i = 0; i < CONFIG_SIZE; i++) {
        if((i != 0) && (i % 8 == 0)) {
            furi_string_cat_printf(temp_config, "\n");
        }
        furi_string_cat_printf(temp_config, "%02X ", instance->gen4_config_display[i]);
    }
    furi_string_cat_printf(temp_config, "\n");

    widget_add_string_element(widget, 3, 4, AlignLeft, AlignTop, FontPrimary, "Gen4 Config");

    widget_add_text_scroll_element(widget, 3, 17, 124, 50, furi_string_get_cstr(temp_config));

    widget_add_button_element(
        widget,
        GuiButtonTypeLeft,
        "Retry",
        nfc_magic_scene_gen4_show_cfg_widget_callback,
        instance);

    furi_string_free(temp_config);
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewWidget);
}

bool nfc_magic_scene_gen4_show_cfg_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            consumed = scene_manager_previous_scene(instance->scene_manager);
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            instance->scene_manager, NfcMagicSceneGen4Menu);
    }
    return consumed;
}

void nfc_magic_scene_gen4_show_cfg_on_exit(void* context) {
    NfcMagicApp* instance = context;

    widget_reset(instance->widget);
}
