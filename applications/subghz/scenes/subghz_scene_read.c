#include "../subghz_i.h"

void subghz_scene_read_callback(SubGhzProtocolCommon* parser, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;

    subghz_history_add_to_history(subghz->txrx->history, parser, subghz->txrx->frequency, subghz->txrx->preset);
    subghz_protocol_reset(subghz->txrx->protocol);
    scene_manager_next_scene(subghz->scene_manager, SubGhzSceneReceiver);
}

void subghz_scene_read_dialog_callback(DialogExResult result, void* context) {
    SubGhz* subghz = context;

    view_dispatcher_send_custom_event(subghz->view_dispatcher, result);
}

const void subghz_scene_read_on_enter(void* context) {
    SubGhz* subghz = context;

    DialogEx* dialog_ex = subghz->dialog_ex;
    dialog_ex_set_left_button_text(dialog_ex, "Config");
    dialog_ex_set_right_button_text(dialog_ex, "Co");

    dialog_ex_set_header(dialog_ex, "Scanning...", 63, 46, AlignLeft, AlignCenter);
    dialog_ex_set_icon(dialog_ex, 0, 0, &I_Scanning_123x52);
    dialog_ex_set_text(dialog_ex, "433.92 AM 00/50", 46, 55, AlignLeft, AlignTop);
    dialog_ex_set_context(dialog_ex, subghz);
    dialog_ex_set_result_callback(dialog_ex, subghz_scene_read_dialog_callback);

    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewDialogEx);

    subghz_begin(subghz->txrx->preset);
    subghz->txrx->frequency = subghz_rx(subghz->txrx->worker, subghz->txrx->frequency);
    subghz_protocol_enable_dump(
        subghz->txrx->protocol, subghz_scene_read_callback, subghz);
}

const bool subghz_scene_read_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == DialogExResultLeft) {
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneReceiverConfig);
            return true;
        } else if(event.event == DialogExResultRight) {
            scene_manager_next_scene(subghz->scene_manager, SubGhzSceneReceiver);
            return true;
        }
    }
    return false;
}

const void subghz_scene_read_on_exit(void* context) {
    SubGhz* subghz = context;

    DialogEx* dialog_ex = subghz->dialog_ex;
    dialog_ex_set_header(dialog_ex, NULL, 0, 0, AlignCenter, AlignCenter);
    dialog_ex_set_text(dialog_ex, NULL, 0, 0, AlignCenter, AlignTop);
    dialog_ex_set_icon(dialog_ex, 0, 0, NULL);
    dialog_ex_set_left_button_text(dialog_ex, NULL);
    dialog_ex_set_right_button_text(dialog_ex, NULL);
    dialog_ex_set_result_callback(dialog_ex, NULL);
    dialog_ex_set_context(dialog_ex, NULL);
}
