#include "../subbrute_i.h"
#include "subbrute_scene.h"

#define TAG "SubBruteSceneStart"

void subbrute_scene_load_select_callback(SubBruteCustomEvent event, void* context) {
    furi_assert(context);

    SubBruteState* instance = (SubBruteState*)context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, event);
}

void subbrute_scene_load_select_on_enter(void* context) {
    furi_assert(context);
#ifdef FURI_DEBUG
    FURI_LOG_I(TAG, "subbrute_scene_load_select_on_enter");
#endif
    SubBruteState* instance = (SubBruteState*)context;
    SubBruteMainView* view = instance->view_main;

    instance->current_view = SubBruteViewMain;
    subbrute_main_view_set_callback(view, subbrute_scene_load_select_callback, instance);
    subbrute_main_view_set_index(
        view,
        7,
        instance->settings->repeat_values,
        true,
        instance->device->two_bytes,
        instance->device->key_from_file);

    view_dispatcher_switch_to_view(instance->view_dispatcher, instance->current_view);
}

void subbrute_scene_load_select_on_exit(void* context) {
    UNUSED(context);
#ifdef FURI_DEBUG
    FURI_LOG_I(TAG, "subbrute_scene_load_select_on_exit");
#endif
}

bool subbrute_scene_load_select_on_event(void* context, SceneManagerEvent event) {
    SubBruteState* instance = (SubBruteState*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubBruteCustomEventTypeIndexSelected) {
            /*#ifdef FURI_DEBUG && !SUBBRUTE_FAST_TRACK
            view_dispatcher_stop(instance->view_dispatcher);
            consumed = true;
#else*/
            instance->device->current_step = 0;
            instance->device->bit_index = subbrute_main_view_get_index(instance->view_main);
            instance->device->two_bytes = subbrute_main_view_get_two_bytes(instance->view_main);

            instance->settings->last_index = instance->device->attack;
            subbrute_settings_set_repeats(
                instance->settings, subbrute_main_view_get_repeats(instance->view_main));
            uint8_t total_repeats = subbrute_settings_get_current_repeats(instance->settings);

            instance->device->max_value = subbrute_protocol_calc_max_value(
                instance->device->attack,
                instance->device->bit_index,
                instance->device->two_bytes);

            if(!subbrute_worker_init_file_attack(
                   instance->worker,
                   instance->device->current_step,
                   instance->device->bit_index,
                   instance->device->key_from_file,
                   instance->device->file_protocol_info,
                   total_repeats,
                   instance->device->two_bytes)) {
                furi_crash("Invalid attack set!");
            }
            subbrute_settings_save(instance->settings);
            scene_manager_next_scene(instance->scene_manager, SubBruteSceneSetupAttack);
            /*#endif*/
            consumed = true;
        } /* else if(event.event == SubBruteCustomEventTypeChangeStepUp) {
            instance->device->two_bytes = true;
        } else if(event.event == SubBruteCustomEventTypeChangeStepDown) {
            instance->device->two_bytes = false;
        }*/
    } else if(event.type == SceneManagerEventTypeBack) {
        if(!scene_manager_search_and_switch_to_previous_scene(
               instance->scene_manager, SubBruteSceneStart)) {
            scene_manager_next_scene(instance->scene_manager, SubBruteSceneStart);
        }
        consumed = true;
    }

    return consumed;
}
