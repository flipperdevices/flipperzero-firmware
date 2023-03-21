#include "../avr_isp_app_i.h"

void avr_isp_scene_load_on_enter(void* context) {
    AvrIspApp* app = context;

    if(avr_isp_load_from_file(app)) {
        scene_manager_next_scene(app->scene_manager, AvrIspSceneWriter);
    } else {
        scene_manager_search_and_switch_to_previous_scene(app->scene_manager, AvrIspSceneStart);
    }
    // if(avr_isp_load_protocol_from_file(subghz)) {
    //     if((!strcmp(subghz->txrx->decoder_result->protocol->name, "RAW"))) {
    //         subghz->txrx->rx_key_state = SubGhzRxKeyStateRAWLoad;
    //         scene_manager_next_scene(subghz->scene_manager, SubGhzSceneReadRAW);
    //     } else {
    //         scene_manager_next_scene(subghz->scene_manager, SubGhzSceneSavedMenu);
    //     }
    // } else {
    //     scene_manager_search_and_switch_to_previous_scene(subghz->scene_manager, SubGhzSceneStart);
    // }
}

bool avr_isp_scene_load_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void avr_isp_scene_load_on_exit(void* context) {
    UNUSED(context);
}
