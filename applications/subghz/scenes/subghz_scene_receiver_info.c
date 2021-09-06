#include "../subghz_i.h"

void subghz_scene_receiver_info_callback(SubghzReceverEvent event, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    view_dispatcher_send_custom_event(subghz->view_dispatcher, event);
}

const void subghz_scene_receiver_info_on_enter(void* context) {
    SubGhz* subghz = context;
    widget_add_string_element(subghz->widget, 10, 42, AlignLeft, AlignTop, FontSecondary, "ffff");
    widget_add_string_element(
        subghz->widget, 20, 10, AlignLeft, AlignTop, FontSecondary, "reciver\r\ninfo");

    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewWidget);
}

const bool subghz_scene_receiver_info_on_event(void* context, SceneManagerEvent event) {
    //SubGhz* subghz = context;
    return false;
}

const void subghz_scene_receiver_info_on_exit(void* context) {
    SubGhz* subghz = context;
    widget_clear(subghz->widget);
}
