#include "../subbrute_i.h"
#include "subbrute_scene.h"

#define TAG "SubBruteSceneStart"

void subbrute_scene_start_callback(SubBruteCustomEvent event, void* context) {
    furi_assert(context);

    SubBruteState* instance = (SubBruteState*)context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, event);
}

void subbrute_scene_start_on_enter(void* context) {
    furi_assert(context);
#ifdef FURI_DEBUG
    FURI_LOG_I(TAG, "subbrute_scene_start_on_enter");
#endif
    SubBruteState* instance = (SubBruteState*)context;
    SubBruteMainView* view = instance->view_main;

    instance->current_view = SubBruteViewMain;
    subbrute_main_view_set_callback(view, subbrute_scene_start_callback, instance);

    instance->device->attack = instance->settings->last_index;

    subbrute_main_view_set_index(
        view,
        instance->settings->last_index,
        instance->settings->repeat_values,
        false,
        instance->device->two_bytes,
        0);

    view_dispatcher_switch_to_view(instance->view_dispatcher, instance->current_view);
}

void subbrute_scene_start_on_exit(void* context) {
    furi_assert(context);
}

bool subbrute_scene_start_on_event(void* context, SceneManagerEvent event) {
    SubBruteState* instance = (SubBruteState*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
#ifdef FURI_DEBUG
        FURI_LOG_D(
            TAG,
            "Event: %ld, SubBruteCustomEventTypeMenuSelected: %s, SubBruteCustomEventTypeLoadFile: %s",
            event.event,
            event.event == SubBruteCustomEventTypeMenuSelected ? "true" : "false",
            event.event == SubBruteCustomEventTypeLoadFile ? "true" : "false");
#endif
        if(event.event == SubBruteCustomEventTypeMenuSelected) {
            instance->settings->last_index = subbrute_main_view_get_index(instance->view_main);
            subbrute_settings_set_repeats(
                instance->settings, subbrute_main_view_get_repeats(instance->view_main));
            uint8_t total_repeats = subbrute_settings_get_current_repeats(instance->settings);

            if((subbrute_device_attack_set(
                    instance->device, instance->settings->last_index, total_repeats) !=
                SubBruteFileResultOk) ||
               (!subbrute_worker_init_default_attack(
                   instance->worker,
                   instance->settings->last_index,
                   instance->device->current_step,
                   instance->device->protocol_info,
                   instance->device->extra_repeats))) {
                furi_crash("Invalid attack set!");
            }
            subbrute_settings_save(instance->settings);
            scene_manager_next_scene(instance->scene_manager, SubBruteSceneSetupAttack);

            consumed = true;
        } else if(event.event == SubBruteCustomEventTypeLoadFile) {
            //uint8_t extra_repeats = subbrute_main_view_get_extra_repeats(instance->view_main);

            //instance->device->extra_repeats = extra_repeats;
            scene_manager_next_scene(instance->scene_manager, SubBruteSceneLoadFile);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        //exit app
        instance->settings->last_index = subbrute_main_view_get_index(instance->view_main);
        subbrute_settings_set_repeats(
            instance->settings, subbrute_main_view_get_repeats(instance->view_main));
        subbrute_settings_save(instance->settings);

        scene_manager_stop(instance->scene_manager);
        view_dispatcher_stop(instance->view_dispatcher);
        consumed = true;
    }

    return consumed;
}
