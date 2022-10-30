#include "../lightmeter.h"

void lightmeter_scene_main_on_enter(void* context) {
    LightMeterApp* lightmeter = context;

    view_dispatcher_switch_to_view(lightmeter->view_dispatcher, VirtualButtonAppViewMainView);
}

bool lightmeter_scene_main_on_event(void* context, SceneManagerEvent event) {
    LightMeterApp* lightmeter = context;
    bool consumed = false;
    UNUSED(lightmeter);

    if(event.type == SceneManagerEventTypeTick) {

        consumed = true;
    }
    return consumed;
}

void lightmeter_scene_main_on_exit(void* context) {
    UNUSED(context);
}
