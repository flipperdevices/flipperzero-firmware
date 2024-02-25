#include "../tullave_i.h"
#include "../lib/tullave/tullave_data.h"

enum TuLlaveReadViewEvent {
    TuLlaveReadViewOnEnter,
    TuLlaveReadViewOnMore,
    TuLlaveReadViewOnShowHistory
};
enum MoreSubmenuIndex { MoreSubmenuShowHistory, MoreSubmenuEmulate };

void tullave_scene_read_widget_callback(GuiButtonType result, InputType type, void* context) {
    TuLlaveApp* instance = context;
    if(type == InputTypeShort && result == GuiButtonTypeRight) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, TuLlaveReadViewOnMore);
    }
}

void tullave_scene_read_success_more_submenu_callback(void* context, uint32_t index) {
    TuLlaveApp* instance = context;
    if(index == MoreSubmenuShowHistory) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, TuLlaveReadViewOnShowHistory);
    }
}

void tullave_scene_read_success_show_card_summary(TuLlaveApp* instance) {
    TuLlavePoller* poller = instance->tullave_poller;
    notification_message(instance->notifications, &sequence_success);
    notification_message(instance->notifications, &sequence_set_green_255);

    widget_add_button_element(
        instance->widget, GuiButtonTypeRight, "More", tullave_scene_read_widget_callback, instance);

    FuriString* widget_text;
    FuriString* uid = furi_string_alloc();
    tullave_data_get_uid_str(poller->card_data, uid);

    widget_text = furi_string_alloc_printf("\e#%s\n", "TuLlave Info");
    furi_string_cat_printf(widget_text, "Uid: %s\n", furi_string_get_cstr(uid));

    furi_string_cat_printf(
        widget_text, "Num: %s\n", furi_string_get_cstr(poller->card_data->card_number));

    furi_string_cat_printf(widget_text, "Balance: $ %lld.00", poller->card_data->balance);

    widget_add_text_scroll_element(
        instance->widget, 0, 0, 128, 52, furi_string_get_cstr(widget_text));
    furi_string_free(widget_text);
    furi_string_free(uid);
}

void tullave_scene_read_success_show_history(TuLlaveApp* instance) {
    widget_reset(instance->widget);

    TuLlaveData* card_data = instance->tullave_poller->card_data;
    FuriString* widget_title = furi_string_alloc_printf("%s", "TuLlave History");
    FuriString* widget_text = furi_string_alloc_set_str("");

    size_t t_count = simple_array_get_count(card_data->transaction_history);
    TuLlaveTransaction* trans_data;
    FuriString* time_str;
    for(size_t i = 0; i < t_count; i++) {
        time_str = furi_string_alloc_set_str("");
        trans_data = simple_array_get(card_data->transaction_history, i);
        tullave_data_format_time(trans_data->datetime, time_str);
        furi_string_cat_printf(widget_text, "%s\n", furi_string_get_cstr(time_str));
        furi_string_cat_printf(widget_text, "Balance: $ %lld.00\n", trans_data->final_balance);
        furi_string_cat_printf(widget_text, "Cost: $ %lld.00\n", trans_data->cost);
        switch(trans_data->type) {
        case BusTravel: {
            furi_string_cat_printf(widget_text, "Type: Bus Travel\n");
            break;
        }
        case CardReload: {
            furi_string_cat_printf(widget_text, "Type: Card Reload\n");
            break;
        }
        case CardReloadCancelled: {
            furi_string_cat_printf(widget_text, "Type: Card Reload Cancelled\n");
            break;
        }
        case UnknownTransaction: {
            furi_string_cat_printf(widget_text, "Type: Unknown Transaction\n");
            break;
        }
        }
        furi_string_cat_printf(widget_text, "---------\n");
        furi_string_free(time_str);
    }

    widget_add_string_element(
        instance->widget,
        0,
        5,
        AlignLeft,
        AlignCenter,
        FontPrimary,
        furi_string_get_cstr(widget_title));
    widget_add_text_scroll_element(
        instance->widget, 0, 15, 128, 52, furi_string_get_cstr(widget_text));

    furi_string_free(widget_text);
    furi_string_free(widget_title);
    view_dispatcher_switch_to_view(instance->view_dispatcher, TuLlaveViewWidget);
}

void tullave_scene_read_success_emulate(TuLlaveApp* instance) {
    UNUSED(instance);
}

void tullave_scene_read_success_show_more_menu(TuLlaveApp* instance) {
    widget_reset(instance->widget);
    submenu_reset(instance->submenu);

    Submenu* submenu = instance->submenu;
    submenu_add_item(
        submenu,
        "Show History",
        MoreSubmenuShowHistory,
        tullave_scene_read_success_more_submenu_callback,
        instance);

    submenu_add_item(
        submenu,
        "Emulate",
        MoreSubmenuEmulate,
        tullave_scene_read_success_more_submenu_callback,
        instance);

    submenu_set_selected_item(submenu, MoreSubmenuShowHistory);
    view_dispatcher_switch_to_view(instance->view_dispatcher, TuLlaveViewMenu);
}

void tullave_scene_read_success_on_enter(void* context) {
    furi_assert(context);
    TuLlaveApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, TuLlaveReadViewOnEnter);
    view_dispatcher_switch_to_view(instance->view_dispatcher, TuLlaveViewWidget);
}

bool tullave_scene_read_success_on_event(void* context, SceneManagerEvent event) {
    TuLlaveApp* instance = context;

    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == TuLlaveReadViewOnEnter) {
            tullave_scene_read_success_show_card_summary(instance);
            consumed = true;
        } else if(event.event == TuLlaveReadViewOnMore) {
            tullave_scene_read_success_show_more_menu(instance);
            consumed = true;
        } else if(event.event == TuLlaveReadViewOnShowHistory) {
            tullave_scene_read_success_show_history(instance);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        // Free data, so a new data could be read.
        tullave_data_free(instance->tullave_poller->card_data);
    }
    return consumed;
}

void tullave_scene_read_success_on_exit(void* context) {
    TuLlaveApp* instance = context;

    widget_reset(instance->widget);
    submenu_reset(instance->submenu);
}