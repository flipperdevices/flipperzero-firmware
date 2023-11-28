#include "../subghz_i.h"
#include "../helpers/subghz_custom_event.h"

void subghz_scene_need_saving_callback(GuiButtonType result, InputType type, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;

    if((result == GuiButtonTypeRight) && (type == InputTypeShort)) {
        view_dispatcher_send_custom_event(subghz->view_dispatcher, SubGhzCustomEventSceneStay);
    } else if((result == GuiButtonTypeLeft) && (type == InputTypeShort)) {
        view_dispatcher_send_custom_event(subghz->view_dispatcher, SubGhzCustomEventSceneExit);
    }
}

void subghz_scene_need_saving_on_enter(void* context) {
    SubGhz* subghz = context;

    widget_add_string_multiline_element(
        subghz->widget,
        64,
        13,
        AlignCenter,
        AlignCenter,
        FontPrimary,
        "Exit and Discard?"); //We honestly dont know where we are goin anymore, so many states!

    widget_add_string_multiline_element(
        subghz->widget,
        64,
        32,
        AlignCenter,
        AlignCenter,
        FontSecondary,
        "All unsaved data\nwill be lost!");

    widget_add_button_element(
        subghz->widget, GuiButtonTypeRight, "Stay", subghz_scene_need_saving_callback, subghz);
    widget_add_button_element(
        subghz->widget, GuiButtonTypeLeft, "Exit", subghz_scene_need_saving_callback, subghz);

    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewIdWidget);
}

bool subghz_scene_need_saving_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;
    if(event.type == SceneManagerEventTypeBack) {
        subghz_rx_key_state_set(subghz, SubGhzRxKeyStateBack);
        scene_manager_previous_scene(subghz->scene_manager);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubGhzCustomEventSceneStay) {
            subghz_rx_key_state_set(subghz, SubGhzRxKeyStateBack);
            scene_manager_previous_scene(subghz->scene_manager);
            return true;
        } else if(event.event == SubGhzCustomEventSceneExit) {
            SubGhzRxKeyState state = subghz_rx_key_state_get(subghz);
            subghz_rx_key_state_set(subghz, SubGhzRxKeyStateIDLE);

            if(scene_manager_has_previous_scene(subghz->scene_manager, SubGhzSceneTransmitter)) {
                if(subghz_key_load(subghz, furi_string_get_cstr(subghz->file_path), false)) {
                    scene_manager_search_and_switch_to_previous_scene(
                        subghz->scene_manager, SubGhzSceneTransmitter);
                } else {
                    //Go to Start Scene!
                    scene_manager_next_scene(subghz->scene_manager, SubGhzSceneStart);
                }
            } else if(scene_manager_has_previous_scene(subghz->scene_manager, SubGhzSceneReadRAW)) {
                if(subghz_key_load(subghz, furi_string_get_cstr(subghz->file_path), false)) {
                    scene_manager_search_and_switch_to_previous_scene(
                        subghz->scene_manager, SubGhzSceneReadRAW);
                } else {
                    //Go to Start Scene!
                    scene_manager_next_scene(subghz->scene_manager, SubGhzSceneStart);
                }
            } else if(state == SubGhzRxKeyStateExit) {
                subghz_txrx_set_preset(
                    subghz->txrx, "AM650", subghz->last_settings->frequency, NULL, 0);
                if(subghz->raw_send_only_old) {
                    scene_manager_previous_scene(subghz->scene_manager);
                } else {
                    scene_manager_search_and_switch_to_previous_scene(
                        subghz->scene_manager, SubGhzSceneStart);
                }
            } else {
                //This is the default exit if you didnt come through Transmitter or Send RAW (Or that Exit State is set!)
                scene_manager_previous_scene(subghz->scene_manager);
            }

            return true;
        }
    }
    return false;
}

void subghz_scene_need_saving_on_exit(void* context) {
    SubGhz* subghz = context;
    widget_reset(subghz->widget);
}
