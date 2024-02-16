#include "../tullave_i.h"

FuriString* tullave_scene_error_parse_err_code(TuLlaveError err_code) {
    FuriString* err_txt = furi_string_alloc();
    switch(err_code) {
    case TuLlaveErrorCardNotSupported: {
        furi_string_set_str(err_txt, "Card is not recognized or not supported.");
        break;
    }
    case TuLlaveErrorCouldNotReadCompleteData: {
        furi_string_set_str(
            err_txt, "Could not read complete card data. Please do not move the card.");
        break;
    }
    case TuLlaveErrorNone: {
        break;
    }
    }

    return err_txt;
}

void tullave_scene_error_on_enter(void* context) {
    furi_assert(context);
    TuLlaveApp* instance = context;

    notification_message(instance->notifications, &sequence_error);
    notification_message(instance->notifications, &sequence_set_red_255);

    FuriString* widget_title =
        furi_string_alloc_printf("\e#TuLlave Error: 0x%03x", instance->tullave_poller->err_code);
    FuriString* widget_text = furi_string_alloc();
    FuriString* err_code_txt =
        tullave_scene_error_parse_err_code(instance->tullave_poller->err_code);

    widget_add_text_box_element(
        instance->widget,
        0,
        0,
        128,
        50,
        AlignLeft,
        AlignTop,
        furi_string_get_cstr(widget_title),
        false);
    widget_add_icon_element(instance->widget, 0, 25, &I_Error_18x18);

    furi_string_cat_printf(widget_text, "%s", furi_string_get_cstr(err_code_txt));

    widget_add_text_scroll_element(
        instance->widget, 25, 25, 100, 50, furi_string_get_cstr(widget_text));

    furi_string_free(widget_title);
    furi_string_free(widget_text);
    furi_string_free(err_code_txt);

    view_dispatcher_switch_to_view(instance->view_dispatcher, TuLlaveViewWidget);
}

bool tullave_scene_error_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void tullave_scene_error_on_exit(void* context) {
    TuLlaveApp* instance = context;

    widget_reset(instance->widget);
}