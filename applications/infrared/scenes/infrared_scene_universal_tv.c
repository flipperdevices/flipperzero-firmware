#include "../infrared_i.h"

#include "common/infrared_scene_universal_common.h"

void infrared_scene_universal_tv_on_enter(void* context) {
    infrared_scene_universal_common_on_enter(context);

    Infrared* infrared = context;
    ButtonPanel* button_panel = infrared->button_panel;
    InfraredBruteForce* brute_force = infrared->brute_force;

    infrared_brute_force_set_db_filename(brute_force, "/ext/infrared/assets/tv.ir");

    button_panel_reserve(button_panel, 2, 3);
    uint32_t i = 0;
    button_panel_add_item(
        button_panel,
        i,
        0,
        0,
        3,
        19,
        &I_Power_25x27,
        &I_Power_hvr_25x27,
        infrared_scene_universal_common_item_callback,
        context);
    infrared_brute_force_add_record(brute_force, i++, "POWER");
    button_panel_add_item(
        button_panel,
        i,
        1,
        0,
        36,
        19,
        &I_Mute_25x27,
        &I_Mute_hvr_25x27,
        infrared_scene_universal_common_item_callback,
        context);
    infrared_brute_force_add_record(brute_force, i++, "MUTE");
    button_panel_add_item(
        button_panel,
        i,
        0,
        1,
        3,
        66,
        &I_Vol_up_25x27,
        &I_Vol_up_hvr_25x27,
        infrared_scene_universal_common_item_callback,
        context);
    infrared_brute_force_add_record(brute_force, i++, "VOL+");
    button_panel_add_item(
        button_panel,
        i,
        1,
        1,
        36,
        66,
        &I_Up_25x27,
        &I_Up_hvr_25x27,
        infrared_scene_universal_common_item_callback,
        context);
    infrared_brute_force_add_record(brute_force, i++, "CH+");
    button_panel_add_item(
        button_panel,
        i,
        0,
        2,
        3,
        98,
        &I_Vol_down_25x27,
        &I_Vol_down_hvr_25x27,
        infrared_scene_universal_common_item_callback,
        context);
    infrared_brute_force_add_record(brute_force, i++, "VOL-");
    button_panel_add_item(
        button_panel,
        i,
        1,
        2,
        36,
        98,
        &I_Down_25x27,
        &I_Down_hvr_25x27,
        infrared_scene_universal_common_item_callback,
        context);
    infrared_brute_force_add_record(brute_force, i++, "CH-");

    button_panel_add_label(button_panel, 6, 11, FontPrimary, "TV remote");
    button_panel_add_label(button_panel, 9, 64, FontSecondary, "Vol");
    button_panel_add_label(button_panel, 43, 64, FontSecondary, "Ch");

    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewStack);

    ViewStack* view_stack = infrared->view_stack;
    Loading* loading = infrared->loading;

    view_stack_add_view(view_stack, loading_get_view(loading));

    /**
     * Problem: Update events are not handled in Loading View, because:
     * 1) Timer task has least prio
     * 2) Storage service uses drivers that capture whole CPU time
     *      to handle SD communication
     *
     * Ugly workaround, but it works for current situation:
     * raise timer task prio for DB scanning period.
     */
    TaskHandle_t timer_task = xTaskGetHandle(configTIMER_SERVICE_TASK_NAME);
    TaskHandle_t storage_task = xTaskGetHandle("StorageSrv");
    uint32_t timer_prio = uxTaskPriorityGet(timer_task);
    uint32_t storage_prio = uxTaskPriorityGet(storage_task);
    vTaskPrioritySet(timer_task, storage_prio + 1);
    bool success = infrared_brute_force_calculate_messages(brute_force);
    vTaskPrioritySet(timer_task, timer_prio);

    view_stack_remove_view(view_stack, loading_get_view(loading));

    if(!success) {
        scene_manager_previous_scene(infrared->scene_manager);
    }
}

bool infrared_scene_universal_tv_on_event(void* context, SceneManagerEvent event) {
    return infrared_scene_universal_common_on_event(context, event);
}

void infrared_scene_universal_tv_on_exit(void* context) {
    infrared_scene_universal_common_on_exit(context);
}
