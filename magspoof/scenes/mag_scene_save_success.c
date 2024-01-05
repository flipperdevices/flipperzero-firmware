#include "../mag_i.h"

void mag_scene_save_success_on_enter(void* context) {
    Mag* mag = context;
    Popup* popup = mag->popup;

    // Clear state of data enter scene
    //scene_manager_set_scene_state(mag->scene_manager, LfRfidSceneSaveData, 0);
    mag_text_store_clear(mag);

    popup_set_icon(popup, 36, 5, &I_DolphinDone_80x58);
    popup_set_header(popup, "Saved!", 5, 7, AlignLeft, AlignTop);
    popup_set_context(popup, mag);
    popup_set_callback(popup, mag_popup_timeout_callback);
    popup_set_timeout(popup, 1500);
    popup_enable_timeout(popup);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewPopup);
}

bool mag_scene_save_success_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    bool consumed = false;

    if((event.type == SceneManagerEventTypeBack) ||
       ((event.type == SceneManagerEventTypeCustom) && (event.event == MagEventPopupClosed))) {
        bool result =
            scene_manager_search_and_switch_to_previous_scene(mag->scene_manager, MagSceneStart);
        if(!result) {
            scene_manager_search_and_switch_to_another_scene(
                mag->scene_manager, MagSceneFileSelect);
        }
        consumed = true;
    }

    return consumed;
}

void mag_scene_save_success_on_exit(void* context) {
    Mag* mag = context;

    popup_reset(mag->popup);
}