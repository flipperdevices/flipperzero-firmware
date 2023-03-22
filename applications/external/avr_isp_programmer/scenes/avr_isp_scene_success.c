#include "../avr_isp_app_i.h"
#include "../views/avr_isp_view_programmer.h"

void avr_isp_scene_success_popup_callback(void* context) {
    AvrIspApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, AvrIspCustomEventSceneSuccess);
}

void avr_isp_scene_success_on_enter(void* context) {
    AvrIspApp* app = context;

    // Setup view
    Popup* popup = app->popup;
    popup_set_icon(popup, 32, 5, &I_DolphinNice_96x59);
    popup_set_header(popup, "Saved!", 13, 22, AlignLeft, AlignBottom);
    popup_set_timeout(popup, 1500);
    popup_set_context(popup, app);
    popup_set_callback(popup, avr_isp_scene_success_popup_callback);
    popup_enable_timeout(popup);
    view_dispatcher_switch_to_view(app->view_dispatcher, AvrIspViewPopup);
}

bool avr_isp_scene_success_on_event(void* context, SceneManagerEvent event) {
    AvrIspApp* app = context;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == AvrIspCustomEventSceneSuccess) {
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, AvrIspSceneStart);
            //     if(!scene_manager_search_and_switch_to_previous_scene(
            //            app -> scene_manager, AvrIspSceneReceiver)) {
            //         app -> txrx->rx_key_state = AvrIspRxKeyStateRAWSave;
            //         if(!scene_manager_search_and_switch_to_previous_scene(
            //                app -> scene_manager, AvrIspSceneReadRAW)) {
            //             app -> txrx->rx_key_state = AvrIspRxKeyStateIDLE;
            //             if(!scene_manager_search_and_switch_to_previous_scene(
            //                    app -> scene_manager, AvrIspSceneSaved)) {
            //                 scene_manager_next_scene(app -> scene_manager, AvrIspSceneSaved);
            //             }
            //         }
            //     }
            return true;
        }
    }
    return false;
}

void avr_isp_scene_success_on_exit(void* context) {
    AvrIspApp* app = context;
    Popup* popup = app->popup;

    popup_reset(popup);
}
