#include "../subbrute_i.h"
#include "subbrute_scene.h"

#define TAG "SubBruteSceneLoadFile"

void subbrute_scene_load_file_on_enter(void* context) {
    furi_assert(context);
    SubBruteState* instance = (SubBruteState*)context;

    // Input events and views are managed by file_browser
    FuriString* app_folder;
    FuriString* load_path;
    load_path = furi_string_alloc();
    app_folder = furi_string_alloc_set(SUBBRUTE_PATH);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, SUBBRUTE_FILE_EXT, &I_sub1_10px);

    SubBruteFileResult load_result = SubBruteFileResultUnknown;
    // TODO: DELETE IT
#ifdef SUBBRUTE_FAST_TRACK
    bool res = true;
    furi_string_printf(load_path, "%s", EXT_PATH("subghz/princeton.sub"));
#else
    bool res =
        dialog_file_browser_show(instance->dialogs, load_path, app_folder, &browser_options);
#endif
#ifdef FURI_DEBUG
    FURI_LOG_D(
        TAG,
        "load_path: %s, app_folder: %s",
        furi_string_get_cstr(load_path),
        furi_string_get_cstr(app_folder));
#endif
    if(res) {
        load_result =
            subbrute_device_load_from_file(instance->device, furi_string_get_cstr(load_path));
        if(load_result == SubBruteFileResultOk) {
            uint8_t extra_repeats = subbrute_main_view_get_extra_repeats(instance->view_main);

            load_result = subbrute_device_attack_set(
                instance->device, SubBruteAttackLoadFile, extra_repeats);
            if(load_result == SubBruteFileResultOk) {
                if(!subbrute_worker_init_file_attack(
                       instance->worker,
                       instance->device->current_step,
                       instance->device->bit_index,
                       instance->device->key_from_file,
                       instance->device->file_protocol_info,
                       extra_repeats,
                       instance->device->two_bytes)) {
                    furi_crash("Invalid attack set!");
                }
                // Ready to run!
                FURI_LOG_I(TAG, "Ready to run");
                res = true;
            }
        }

        if(load_result == SubBruteFileResultOk) {
            scene_manager_next_scene(instance->scene_manager, SubBruteSceneLoadSelect);
        } else {
            FURI_LOG_E(TAG, "Returned error: %d", load_result);

            FuriString* dialog_msg;
            dialog_msg = furi_string_alloc();
            furi_string_cat_printf(
                dialog_msg, "Cannot parse\nfile: %s", subbrute_device_error_get_desc(load_result));
            dialog_message_show_storage_error(instance->dialogs, furi_string_get_cstr(dialog_msg));
            furi_string_free(dialog_msg);
            scene_manager_search_and_switch_to_previous_scene(
                instance->scene_manager, SubBruteSceneStart);
        }
    } else {
        scene_manager_search_and_switch_to_previous_scene(
            instance->scene_manager, SubBruteSceneStart);
    }

    furi_string_free(app_folder);
    furi_string_free(load_path);
}

void subbrute_scene_load_file_on_exit(void* context) {
    UNUSED(context);
}

bool subbrute_scene_load_file_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}