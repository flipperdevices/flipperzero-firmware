#include "../infrared_i.h"

#include "common/infrared_scene_universal_common.h"

void infrared_scene_universal_ac_on_enter(void* context) {
    infrared_scene_universal_common_on_enter(context);

    Infrared* infrared = context;
    ButtonPanel* button_panel = infrared->button_panel;

    button_panel_reserve(button_panel, 2, 2);
    uint32_t i = 0;
    button_panel_add_item(
        button_panel,
        i,
        0,
        0,
        4,
        67,
        &I_AC_off_25x27,
        &I_AC_off_hvr_25x27,
        infrared_scene_universal_common_item_callback,
        context);
    button_panel_add_item(
        button_panel,
        i,
        1,
        0,
        35,
        67,
        &I_Up_25x27,
        &I_Up_hvr_25x27,
        infrared_scene_universal_common_item_callback,
        context);
    button_panel_add_item(
        button_panel,
        i,
        0,
        1,
        4,
        99,
        &I_AC_set_25x27,
        &I_AC_set_hvr_25x27,
        infrared_scene_universal_common_item_callback,
        context);
    button_panel_add_item(
        button_panel,
        i,
        1,
        1,
        35,
        99,
        &I_Down_25x27,
        &I_Down_hvr_25x27,
        infrared_scene_universal_common_item_callback,
        context);

    button_panel_add_label(button_panel, 6, 8, FontPrimary, "AC remote");
    button_panel_add_label(button_panel, 2, 20, FontSecondary, "Choose and set");
    button_panel_add_label(button_panel, 5, 30, FontSecondary, "temperature");
    button_panel_add_label(button_panel, 10, 53, FontPrimary, "Set: 23 c");

    view_set_orientation(view_stack_get_view(infrared->view_stack), ViewOrientationVertical);
    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewStack);
}

bool infrared_scene_universal_ac_on_event(void* context, SceneManagerEvent event) {
    return infrared_scene_universal_common_on_event(context, event);
}

void infrared_scene_universal_ac_on_exit(void* context) {
    infrared_scene_universal_common_on_exit(context);
}
