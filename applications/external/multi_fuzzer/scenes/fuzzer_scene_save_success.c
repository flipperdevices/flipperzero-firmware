#include "../fuzzer_i.h"

static void fuzzer_scene_save_popup_timeout_callback(void* context) {
    PacsFuzzerApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, FuzzerCustomEventPopupClosed);
}

void fuzzer_scene_save_success_on_enter(void* context) {
    PacsFuzzerApp* app = context;
    Popup* popup = app->popup;

    popup_set_icon(popup, 32, 5, &I_DolphinNice_96x59);
    popup_set_header(popup, "Saved!", 5, 7, AlignLeft, AlignTop);
    popup_set_context(popup, app);
    popup_set_callback(popup, fuzzer_scene_save_popup_timeout_callback);
    popup_set_timeout(popup, 1500);
    popup_enable_timeout(popup);

    view_dispatcher_switch_to_view(app->view_dispatcher, FuzzerViewIDPopup);
}

bool fuzzer_scene_save_success_on_event(void* context, SceneManagerEvent event) {
    PacsFuzzerApp* app = context;
    bool consumed = false;

    if((event.type == SceneManagerEventTypeBack) ||
       ((event.type == SceneManagerEventTypeCustom) &&
        (event.event == FuzzerCustomEventPopupClosed))) {
        bool result = scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, FuzzerSceneAttack);
        if(!result) {
            scene_manager_search_and_switch_to_previous_scene(app->scene_manager, FuzzerSceneMain);
        }
        consumed = true;
    }

    return consumed;
}

void fuzzer_scene_save_success_on_exit(void* context) {
    PacsFuzzerApp* app = context;

    popup_reset(app->popup);
}
