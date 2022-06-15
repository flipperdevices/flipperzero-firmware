#include "../infrared_i.h"

void infrared_scene_universal_tv_on_enter(void* context) {
    Infrared* infrared = context;
    ButtonPanel* button_panel = infrared->button_panel;
    InfraredBruteForce* brute_force = infrared->brute_force;

    button_panel_reserve(button_panel, 2, 3);
    uint32_t i = 0;
    button_panel_add_item(
        button_panel, i, 0, 0, 3, 19, &I_Power_25x27, &I_Power_hvr_25x27, NULL, context);
    infrared_brute_force_add_record(brute_force, i++, "POWER");
    button_panel_add_item(
        button_panel, i, 1, 0, 36, 19, &I_Mute_25x27, &I_Mute_hvr_25x27, NULL, context);
    infrared_brute_force_add_record(brute_force, i++, "MUTE");
    button_panel_add_item(
        button_panel, i, 0, 1, 3, 66, &I_Vol_up_25x27, &I_Vol_up_hvr_25x27, NULL, context);
    infrared_brute_force_add_record(brute_force, i++, "VOL+");
    button_panel_add_item(
        button_panel, i, 1, 1, 36, 66, &I_Up_25x27, &I_Up_hvr_25x27, NULL, context);
    infrared_brute_force_add_record(brute_force, i++, "CH+");
    button_panel_add_item(
        button_panel, i, 0, 2, 3, 98, &I_Vol_down_25x27, &I_Vol_down_hvr_25x27, NULL, context);
    infrared_brute_force_add_record(brute_force, i++, "VOL-");
    button_panel_add_item(
        button_panel, i, 1, 2, 36, 98, &I_Down_25x27, &I_Down_hvr_25x27, NULL, context);
    infrared_brute_force_add_record(brute_force, i++, "CH-");

    button_panel_add_label(button_panel, 6, 11, FontPrimary, "TV remote");
    button_panel_add_label(button_panel, 9, 64, FontSecondary, "Vol");
    button_panel_add_label(button_panel, 43, 64, FontSecondary, "Ch");

    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewStack);
}

bool infrared_scene_universal_tv_on_event(void* context, SceneManagerEvent event) {
    Infrared* infrared = context;
    UNUSED(infrared);
    UNUSED(event);
    bool consumed = false;

    return consumed;
}

void infrared_scene_universal_tv_on_exit(void* context) {
    Infrared* infrared = context;
    UNUSED(infrared);
}
