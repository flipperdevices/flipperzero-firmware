#include "../infrared_i.h"

#include "common/infrared_scene_universal_common.h"

typedef enum {
    InfraredUniversalACButtonOff,
    InfraredUniversalACButtonSet,
    InfraredUniversalACButtonUp,
    InfraredUniversalACButtonDown,
} InfraredUniversalACButton;

static const int8_t infrared_scene_universal_ac_temps[] = {20, 23, 25, 28};

static void infrared_scene_universal_ac_update_temp(Infrared* infrared) {
    uint32_t selected_temp_index =
        scene_manager_get_scene_state(infrared->scene_manager, InfraredSceneUniversalAC);
    infrared_text_store_set(
        infrared, 0, "Set: %d C", infrared_scene_universal_ac_temps[selected_temp_index]);
}

void infrared_scene_universal_ac_on_enter(void* context) {
    infrared_scene_universal_common_on_enter(context);

    Infrared* infrared = context;
    SceneManager* scene_manager = infrared->scene_manager;
    ButtonPanel* button_panel = infrared->button_panel;
    InfraredBruteForce* brute_force = infrared->brute_force;

    scene_manager_set_scene_state(scene_manager, InfraredSceneUniversalAC, 0);
    infrared_brute_force_set_db_filename(brute_force, EXT_PATH("infrared/assets/ac.ir"));

    for(size_t i = 0; i < COUNT_OF(infrared_scene_universal_ac_temps); ++i) {
        char tmp[10];
        snprintf(tmp, sizeof(tmp), "SET%dC", infrared_scene_universal_ac_temps[i]);
        infrared_brute_force_add_record(brute_force, i, tmp);
    }

    infrared_brute_force_add_record(
        brute_force, COUNT_OF(infrared_scene_universal_ac_temps), "OFF");

    button_panel_reserve(button_panel, 2, 2);
    button_panel_add_item(
        button_panel,
        InfraredUniversalACButtonOff,
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
        InfraredUniversalACButtonUp,
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
        InfraredUniversalACButtonSet,
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
        InfraredUniversalACButtonDown,
        1,
        1,
        35,
        99,
        &I_Down_25x27,
        &I_Down_hvr_25x27,
        infrared_scene_universal_common_item_callback,
        context);

    infrared_scene_universal_ac_update_temp(infrared);

    button_panel_add_label(button_panel, 6, 8, FontPrimary, "AC remote");
    button_panel_add_label(button_panel, 2, 20, FontSecondary, "Choose and set");
    button_panel_add_label(button_panel, 5, 30, FontSecondary, "temperature");
    button_panel_add_label(button_panel, 10, 53, FontPrimary, infrared->text_store[0]);
    button_panel_add_frame(button_panel, 0, 36, 64, 26);

    view_set_orientation(view_stack_get_view(infrared->view_stack), ViewOrientationVertical);
    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewStack);

    infrared_show_loading_popup(infrared, true);
    bool success = infrared_brute_force_calculate_messages(brute_force);
    infrared_show_loading_popup(infrared, false);

    if(!success) {
        scene_manager_next_scene(infrared->scene_manager, InfraredSceneErrorDatabases);
    }
}

bool infrared_scene_universal_ac_on_event(void* context, SceneManagerEvent event) {
    bool consumed = false;
    Infrared* infrared = context;
    SceneManager* scene_manager = infrared->scene_manager;

    if(event.type == SceneManagerEventTypeCustom) {
        uint16_t custom_event_type;
        int16_t custom_event_value;
        uint32_t selected_temp_index =
            scene_manager_get_scene_state(scene_manager, InfraredSceneUniversalAC);

        infrared_custom_event_unpack(event.event, &custom_event_type, &custom_event_value);
        if(custom_event_type == InfraredCustomEventTypeButtonSelected) {
            if(custom_event_value == InfraredUniversalACButtonUp ||
               custom_event_value == InfraredUniversalACButtonDown) {
                bool is_temp_changed = false;
                if(custom_event_value == InfraredUniversalACButtonUp) {
                    is_temp_changed = selected_temp_index <
                                      COUNT_OF(infrared_scene_universal_ac_temps) - 1;
                    if(is_temp_changed) {
                        ++selected_temp_index;
                    }
                } else if(custom_event_value == InfraredUniversalACButtonDown) {
                    is_temp_changed = selected_temp_index > 0;
                    if(is_temp_changed) {
                        --selected_temp_index;
                    }
                }
                if(is_temp_changed) {
                    scene_manager_set_scene_state(
                        scene_manager, InfraredSceneUniversalAC, selected_temp_index);
                    infrared_scene_universal_ac_update_temp(infrared);
                }
                consumed = true;

            } else if(
                custom_event_value == InfraredUniversalACButtonOff ||
                custom_event_value == InfraredUniversalACButtonSet) {
                if(custom_event_value == InfraredUniversalACButtonOff) {
                    event.event = infrared_custom_event_pack(
                        custom_event_type, COUNT_OF(infrared_scene_universal_ac_temps));
                } else if(custom_event_value == InfraredUniversalACButtonSet) {
                    event.event =
                        infrared_custom_event_pack(custom_event_type, selected_temp_index);
                }
                consumed = infrared_scene_universal_common_on_event(context, event);
            }
        }
    } else {
        consumed = infrared_scene_universal_common_on_event(context, event);
    }
    return consumed;
}

void infrared_scene_universal_ac_on_exit(void* context) {
    infrared_scene_universal_common_on_exit(context);
}
