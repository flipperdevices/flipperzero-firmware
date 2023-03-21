#include "../avr_isp_app_i.h"
#include "../views/avr_isp_view_programmer.h"

void avr_isp_scene_programmer_callback(AvrIspCustomEvent event, void* context) {
    furi_assert(context);
    AvrIspApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void avr_isp_scene_programmer_on_enter(void* context) {
    AvrIspApp* app = context;

    avr_isp_programmer_view_set_callback(
        app->avr_isp_programmer_view, avr_isp_scene_programmer_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, AvrIspViewProgrammer);
}

bool avr_isp_scene_programmer_on_event(void* context, SceneManagerEvent event) {
    AvrIspApp* app = context;
    UNUSED(app);
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        // case WSCustomEventViewprogrammerBack:
        //     consumed = true;
        //     break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
    }
    return consumed;
}

void avr_isp_scene_programmer_on_exit(void* context) {
    UNUSED(context);
}
