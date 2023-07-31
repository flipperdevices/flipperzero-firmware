#include "../uhf_app_i.h"
#include <dolphin/dolphin.h>

void uhf_scene_save_success_popup_callback(void* context) {
    UHFApp* uhf_app = context;
    view_dispatcher_send_custom_event(uhf_app->view_dispatcher, UHFCustomEventViewExit);
}

void uhf_scene_save_success_on_enter(void* context) {
    UHFApp* uhf_app = context;
    dolphin_deed(DolphinDeedNfcSave);

    // Setup view
    Popup* popup = uhf_app->popup;
    popup_set_icon(popup, 32, 5, &I_DolphinNice_96x59);
    popup_set_header(popup, "Saved!", 13, 22, AlignLeft, AlignBottom);
    popup_set_timeout(popup, 1500);
    popup_set_context(popup, uhf_app);
    popup_set_callback(popup, uhf_scene_save_success_popup_callback);
    popup_enable_timeout(popup);
    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewPopup);
}

bool uhf_scene_save_success_on_event(void* context, SceneManagerEvent event) {
    UHFApp* uhf_app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UHFCustomEventViewExit) {
            if(scene_manager_has_previous_scene(uhf_app->scene_manager, UHFSceneCardMenu)) {
                consumed = scene_manager_search_and_switch_to_previous_scene(
                    uhf_app->scene_manager, UHFSceneCardMenu);
            } else {
                consumed = scene_manager_search_and_switch_to_previous_scene(
                    uhf_app->scene_manager, UHFSceneStart);
            }
        }
    }
    return consumed;
}

void uhf_scene_save_success_on_exit(void* context) {
    UHFApp* uhf_app = context;

    // Clear view
    popup_reset(uhf_app->popup);
}
