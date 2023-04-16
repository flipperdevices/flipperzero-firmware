#include "../seader_i.h"

void seader_scene_delete_success_popup_callback(void* context) {
    Seader* seader = context;
    view_dispatcher_send_custom_event(seader->view_dispatcher, SeaderCustomEventViewExit);
}

void seader_scene_delete_success_on_enter(void* context) {
    Seader* seader = context;

    // Setup view
    Popup* popup = seader->popup;
    popup_set_icon(popup, 0, 2, &I_DolphinMafia_115x62);
    popup_set_header(popup, "Deleted", 83, 19, AlignLeft, AlignBottom);
    popup_set_timeout(popup, 1500);
    popup_set_context(popup, seader);
    popup_set_callback(popup, seader_scene_delete_success_popup_callback);
    popup_enable_timeout(popup);
    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewPopup);
}

bool seader_scene_delete_success_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SeaderCustomEventViewExit) {
            consumed = scene_manager_search_and_switch_to_previous_scene(
                seader->scene_manager, SeaderSceneStart);
        }
    }
    return consumed;
}

void seader_scene_delete_success_on_exit(void* context) {
    Seader* seader = context;

    // Clear view
    popup_reset(seader->popup);
}
