#include "../avr_isp_app_i.h"
#include "../views/avr_isp_view_chip_detect.h"

void avr_isp_scene_chip_detect_callback(AvrIspCustomEvent event, void* context) {
    furi_assert(context);
    AvrIspApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void avr_isp_scene_chip_detect_on_enter(void* context) {
    AvrIspApp* app = context;

    // avr_isp_chip_detect_set_file_path(
    //     app->avr_isp_chip_detect_view, furi_string_get_cstr(app->file_path), app->file_name_tmp);
    avr_isp_chip_detect_view_set_callback(
        app->avr_isp_chip_detect_view, avr_isp_scene_chip_detect_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, AvrIspViewChipDetect);
}

bool avr_isp_scene_chip_detect_on_event(void* context, SceneManagerEvent event) {
    AvrIspApp* app = context;
    UNUSED(app);
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case AvrIspCustomEventSceneChipDetectOk:
             FURI_LOG_E("FF","DETECL OK");
            consumed = true;
            break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
    }
    return consumed;
}

void avr_isp_scene_chip_detect_on_exit(void* context) {
    UNUSED(context);
}
