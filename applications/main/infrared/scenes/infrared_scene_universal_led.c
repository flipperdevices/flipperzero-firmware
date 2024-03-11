#include "../infrared_app_i.h"

#include "common/infrared_scene_universal_common.h"

void infrared_scene_universal_led_on_enter(void* context) {
    InfraredApp* infrared = context;
    ButtonPanel* button_panel = infrared->button_panel;
    InfraredBruteForce* brute_force = infrared->brute_force;

    infrared_brute_force_set_db_filename(brute_force, EXT_PATH("infrared/assets/led.ir"));

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
        37,
        21,
        &I_volup_24x21,
        &I_volup_hover_24x21,
        infrared_scene_universal_common_item_callback,
        context);
    infrared_brute_force_add_record(brute_force, i++, "BRIGHTNESS+");

    button_panel_add_item(
        button_panel,
        i,
        1,
        1,
        37,
        55,
        &I_voldown_24x21,
        &I_voldown_hover_24x21,
        infrared_scene_universal_common_item_callback,
        context);
    infrared_brute_force_add_record(brute_force, i++, "BRIGHTNESS-");

    button_panel_add_icon(button_panel, 34, 34, &I_bright_text_30x30);

    button_panel_add_item(
        button_panel,
        i,
        0,
        1,
        6,
        53,
        &I_flash_19x20,
        &I_flash_hover_19x20,
        infrared_scene_universal_common_item_callback,
        context);
    button_panel_add_icon(button_panel, 6, 75, &I_flash_text_21x5);
    infrared_brute_force_add_record(brute_force, i++, "FLASH");

    button_panel_add_label(button_panel, 21, 11, FontPrimary, "LEDs");

    infrared_scene_universal_common_on_enter(context);
}

bool infrared_scene_universal_led_on_event(void* context, SceneManagerEvent event) {
    return infrared_scene_universal_common_on_event(context, event);
}

void infrared_scene_universal_led_on_exit(void* context) {
    infrared_scene_universal_common_on_exit(context);
}
