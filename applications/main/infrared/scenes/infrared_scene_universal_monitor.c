#include "../infrared_app_i.h"

#include "common/infrared_scene_universal_common.h"

void infrared_scene_universal_monitor_on_enter(void* context) {
    InfraredApp* infrared = context;
    ButtonPanel* button_panel = infrared->button_panel;
    InfraredBruteForce* brute_force = infrared->brute_force;

    infrared_brute_force_set_db_filename(brute_force, EXT_PATH("infrared/assets/monitor.ir"));

    //TODO: Add navigation buttons, add more monitors..
    button_panel_reserve(button_panel, 2, 2);
    uint32_t i = 0;
    button_panel_add_item(
        button_panel,
        i,
        0,
        0,
        6,
        24,
        &I_power_19x20,
        &I_power_hover_19x20,
        infrared_scene_universal_common_item_callback,
        context);
    button_panel_add_icon(button_panel, 4, 46, &I_power_text_24x5);
    infrared_brute_force_add_record(brute_force, i++, "POWER");

    button_panel_add_item(
        button_panel,
        i,
        1,
        0,
        38,
        24,
        &I_input_19x20,
        &I_input_hover_19x20,
        infrared_scene_universal_common_item_callback,
        context);
    button_panel_add_icon(button_panel, 36, 46, &I_input_text_24x5);
    infrared_brute_force_add_record(brute_force, i++, "SOURCE");

    button_panel_add_item(
        button_panel,
        i,
        0,
        1,
        6,
        58,
        &I_mode_19x20,
        &I_mode_hover_19x20,
        infrared_scene_universal_common_item_callback,
        context);
    button_panel_add_icon(button_panel, 6, 80, &I_menu_text_20x5);
    infrared_brute_force_add_record(brute_force, i++, "MENU");

    button_panel_add_item(
        button_panel,
        i,
        1,
        1,
        38,
        58,
        &I_exit_19x20,
        &I_exit_hover_19x20,
        infrared_scene_universal_common_item_callback,
        context);
    button_panel_add_icon(button_panel, 39, 80, &I_exit_text_18x5);
    infrared_brute_force_add_record(brute_force, i++, "EXIT");

    button_panel_add_label(button_panel, 10, 11, FontPrimary, "Monitors");

    infrared_scene_universal_common_on_enter(context);
}

bool infrared_scene_universal_monitor_on_event(void* context, SceneManagerEvent event) {
    return infrared_scene_universal_common_on_event(context, event);
}

void infrared_scene_universal_monitor_on_exit(void* context) {
    infrared_scene_universal_common_on_exit(context);
}
