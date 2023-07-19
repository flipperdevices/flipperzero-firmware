#include "../seader_i.h"
#include <dolphin/dolphin.h>

void seader_scene_save_success_popup_callback(void* context) {
    Seader* seader = context;
    view_dispatcher_send_custom_event(seader->view_dispatcher, SeaderCustomEventViewExit);
}

void seader_scene_save_success_on_enter(void* context) {
    Seader* seader = context;
    dolphin_deed(DolphinDeedNfcSave);

    // Setup view
    Popup* popup = seader->popup;
    popup_set_icon(popup, 32, 5, &I_DolphinNice_96x59);
    popup_set_header(popup, "Saved!", 13, 22, AlignLeft, AlignBottom);
    popup_set_timeout(popup, 1500);
    popup_set_context(popup, seader);
    popup_set_callback(popup, seader_scene_save_success_popup_callback);
    popup_enable_timeout(popup);
    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewPopup);
}

bool seader_scene_save_success_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SeaderCustomEventViewExit) {
            if(scene_manager_has_previous_scene(seader->scene_manager, SeaderSceneCardMenu)) {
                consumed = scene_manager_search_and_switch_to_previous_scene(
                    seader->scene_manager, SeaderSceneCardMenu);
            } else {
                consumed = scene_manager_search_and_switch_to_previous_scene(
                    seader->scene_manager, SeaderSceneSamPresent);
            }
        }
    }
    return consumed;
}

void seader_scene_save_success_on_exit(void* context) {
    Seader* seader = context;

    // Clear view
    popup_reset(seader->popup);
}
