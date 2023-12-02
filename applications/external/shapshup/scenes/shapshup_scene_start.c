#include "../shapshup_i.h"

#define TAG "ShapShupSceneStart"

void shapshup_scene_start_callback(ShapshupCustomEvent event, void* context) {
    furi_assert(context);

    ShapShupState* instance = (ShapShupState*)context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, event);
}

void shapshup_scene_start_on_enter(void* context) {
    furi_assert(context);

    ShapShupState* instance = (ShapShupState*)context;
    ShapShupMainView* view = instance->view_main;
    instance->current_view = ShapShupViewMain;

    shapshup_main_view_set_callback(view, shapshup_scene_start_callback, instance);
    view_dispatcher_switch_to_view(instance->view_dispatcher, instance->current_view);

    // Goto load file if no file is selected
    if(shapshup_main_view_no_file(view)) {
        scene_manager_next_scene(instance->scene_manager, ShapshupSceneLoadFile);
    }
}

void shapshup_scene_start_on_exit(void* context) {
    UNUSED(context);
}

bool shapshup_scene_start_on_event(void* context, SceneManagerEvent event) {
    ShapShupState* instance = (ShapShupState*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == ShapShupCustomEventTypeMenuSelected) {
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "ShapShupCustomEventTypeMenuSelected");
#endif
            // FuriString* value;
            // value = furi_string_alloc_printf("%ld", rand_range(100, 600));
            // shapshup_main_view_add_item(instance->view_main, furi_string_get_cstr(value), 0);
            // furi_string_free(value);

            // FlipperFormat* flipper_format = flipper_format_string_alloc();
            // Stream* stream = flipper_format_get_raw_stream(flipper_format);
            // if(stream_load_from_file(
            //        stream, instance->storage, EXT_PATH("subghz/temp/split.sub")) <= 0) {
            //     FURI_LOG_E(TAG, "Error loading file");
            // }

            // write_file_split_shapshup(
            //     instance->storage, flipper_format, EXT_PATH("subghz/temp/split_proc.sub"));

            // flipper_format_free(flipper_format);

            consumed = true;
        } else if(event.event == ShapShupCustomEventTypeLoadFile) {
#ifdef FURI_DEBUG
            FURI_LOG_D(TAG, "ShapShupCustomEventTypeLoadFile");
#endif
            scene_manager_next_scene(instance->scene_manager, ShapshupSceneLoadFile);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(instance->scene_manager);
        view_dispatcher_stop(instance->view_dispatcher);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        shapshup_main_view_check_alert(instance->view_main);
        consumed = true;
    }

    return consumed;
}
