#include "../tullave_i.h"

void tullave_scene_read_widget_callback(GuiButtonType result, InputType type, void* context) {
    TuLlaveApp* instance = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, result);
    }
}

void tullave_scene_read_success_on_enter(void* context) {
    TuLlaveApp* instance = context;

    notification_message(instance->notifications, &sequence_success);
    notification_message(instance->notifications, &sequence_set_green_255);

    widget_add_button_element(
        instance->widget, GuiButtonTypeRight, "More", tullave_scene_read_widget_callback, instance);

    FuriString* widget_text;

    widget_text = furi_string_alloc_printf("\e#%s\n", "TuLlave Info");
    furi_string_cat_printf(
        widget_text, "Uid: %s\n", furi_string_get_cstr(instance->card_data->nfc_uid));

    furi_string_cat_printf(
        widget_text, "Num: %s\n", furi_string_get_cstr(instance->card_data->card_number));

    furi_string_cat_printf(
        widget_text,
        "Balance: $ %lld.%02lld",
        instance->card_data->balance,
        instance->card_data->balance % 100);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 52, furi_string_get_cstr(widget_text));
    furi_string_free(widget_text);

    view_dispatcher_switch_to_view(instance->view_dispatcher, TuLlaveViewWidget);
}

bool tullave_scene_read_success_on_event(void* context, SceneManagerEvent event) {
    TuLlaveApp* instance = context;
    if(event.type == SceneManagerEventTypeBack) {
        // Free data, so a new data could be read.
        tullave_data_free(instance->card_data);
    }
    return false;
}

void tullave_scene_read_success_on_exit(void* context) {
    TuLlaveApp* instance = context;

    widget_reset(instance->widget);
}