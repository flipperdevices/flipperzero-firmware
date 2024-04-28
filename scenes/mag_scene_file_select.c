#include "../mag_i.h"
#include "../mag_device.h"

void mag_scene_file_select_on_enter(void* context) {
    Mag* mag = context;
    mag_device_set_loading_callback(mag->mag_dev, mag_show_loading_popup, mag);
    if(mag_file_select(mag->mag_dev)) {
        MagTrackState auto_track = mag_device_autoselect_track_state(mag->mag_dev);
        if(auto_track) {
            mag->state.track = auto_track;
        }
        scene_manager_next_scene(mag->scene_manager, MagSceneSavedMenu);
    } else {
        scene_manager_search_and_switch_to_previous_scene(mag->scene_manager, MagSceneStart);
    }
    mag_device_set_loading_callback(mag->mag_dev, NULL, mag);
}

bool mag_scene_file_select_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void mag_scene_file_select_on_exit(void* context) {
    UNUSED(context);
}