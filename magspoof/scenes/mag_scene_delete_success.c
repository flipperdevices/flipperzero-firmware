#include "../mag_i.h"

void mag_scene_delete_success_on_enter(void* context) {
    Mag* mag = context;
    Popup* popup = mag->popup;

    popup_set_icon(popup, 0, 2, &I_DolphinMafia_119x62);
    popup_set_header(popup, "Deleted", 83, 19, AlignLeft, AlignBottom);

    popup_set_callback(popup, mag_popup_timeout_callback);
    popup_set_context(popup, mag);
    popup_set_timeout(popup, 1500);
    popup_enable_timeout(popup);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewPopup);
}

bool mag_scene_delete_success_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MagEventPopupClosed) {
            consumed = true;

            scene_manager_search_and_switch_to_previous_scene(
                mag->scene_manager, MagSceneFileSelect);
        }
    }

    return consumed;
}

void mag_scene_delete_success_on_exit(void* context) {
    Mag* mag = context;
    Popup* popup = mag->popup;

    popup_reset(popup);
}