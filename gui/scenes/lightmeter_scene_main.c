#include "../../lightmeter.h"

static void lightmeter_scene_main_on_left(void* context) {
    LightMeterApp* lightmeter = (LightMeterApp*)context;
    view_dispatcher_send_custom_event(lightmeter->view_dispatcher, LightMeterAppCustomEventConfig);
}

void lightmeter_scene_main_on_enter(void* context) {
    LightMeterApp* lightmeter = context;
    lightmeter_main_view_set_left_callback(lightmeter->main_view, lightmeter_scene_main_on_left, lightmeter);
    view_dispatcher_switch_to_view(lightmeter->view_dispatcher, LightMeterAppViewMainView);
}

bool lightmeter_scene_main_on_event(void* context, SceneManagerEvent event) {
    LightMeterApp* lightmeter = context;
    // bool consumed = false;
    UNUSED(lightmeter);

    // if(event.type == SceneManagerEventTypeCustom) {
    //     // if(event.event == LightMeterSubmenuIndexConfigView) {
    //     // }
    // } else if(event.type == SceneManagerEventTypeTick) {
    //     consumed = true;
    // }
    // return consumed;



    //  DapGuiApp* app = context;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == LightMeterAppCustomEventConfig) {
            scene_manager_next_scene(lightmeter->scene_manager, LightMeterAppSceneConfig);
            return true;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        // return process_dap_state(app);
        return true;
    }

    return false;
}



void lightmeter_scene_main_on_exit(void* context) {
    UNUSED(context);
}
