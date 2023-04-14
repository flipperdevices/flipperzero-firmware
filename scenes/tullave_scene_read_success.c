#include "../tullave_i.h"

void tullave_scene_read_success_widget_callback(
    GuiButtonType result,
    InputType type,
    void* context) {
    TuLlave* t_llave = context;

    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(t_llave->view_dispatcher, result);
    }
}

static void tullave_scene_read_success_setup_view(TuLlave* t_llave) {
    widget_add_button_element(
        t_llave->widget,
        GuiButtonTypeRight,
        "More",
        tullave_scene_read_success_widget_callback,
        t_llave);

    FuriString* widget_text;
    TuLlaveInfo* card_info = t_llave->worker->card_info;
    widget_text = furi_string_alloc_printf("\e#%s\n\n", "TuLlave Info");

    furi_string_cat_printf(widget_text, "Id: %s\n", furi_string_get_cstr(card_info->card_number));

    furi_string_cat_printf(
        widget_text, "Balance: $ %lld.%02lld", card_info->balance, card_info->balance % 100);

    widget_add_text_scroll_element(
        t_llave->widget, 0, 0, 128, 52, furi_string_get_cstr(widget_text));
    furi_string_free(widget_text);

    view_dispatcher_switch_to_view(t_llave->view_dispatcher, TuLlaveViewWidget);
}

void tullave_scene_read_success_on_enter(void* context) {
    TuLlave* t_llave = context;
    tullave_scene_read_success_setup_view(t_llave);
}

bool tullave_scene_read_success_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);

    return false;
}

void tullave_scene_read_success_on_exit(void* context) {
    TuLlave* t_llave = context;

    widget_reset(t_llave->widget);
}