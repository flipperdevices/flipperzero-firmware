#include "gas_sensor_scene.h"
#include "../gas_sensor_i.h"

#include <furi.h>

#define TAG "gas_sensor_scene_sensor_info"

static void gas_sensor_scene_sensor_info_set_text(GasSensorApp* gsa) {
    furi_assert(gsa);
    gas_sensor_update_sensors(gsa);
    popup_reset(gsa->popup);
    popup_set_header(gsa->popup, "Air & Gas Measurements", 2, 8, AlignLeft, AlignBottom);
    popup_set_text(gsa->popup, gas_sensor_get_text_buffer(gsa), 2, 14, AlignLeft, AlignTop);
}

void gas_sensor_scene_sensor_info_on_enter(void* context) {
    FURI_LOG_T(TAG, "on_enter");
    furi_assert(context);
    GasSensorApp* gsa = context;
    popup_reset(gsa->popup);
    gas_sensor_scene_sensor_info_set_text(gsa);
    view_dispatcher_switch_to_view(gsa->view_dispatcher, GasSensorViewSensorInfo);
}

bool gas_sensor_scene_sensor_info_on_event(void* context, SceneManagerEvent event) {
    furi_assert(context);
    GasSensorApp* gsa = context;
    bool consumed = false;
    // Sgp30Readings readings;

    if(event.type == SceneManagerEventTypeTick) {
        uint32_t current_tick = furi_get_tick();
        if((current_tick - gsa->last_update_tick) > 1000) {
            FURI_LOG_T(TAG, "updating view text");
            gas_sensor_scene_sensor_info_set_text(gsa);
            consumed = true;
        }
    }

    return consumed;
}

void gas_sensor_scene_sensor_info_on_exit(void* context) {
    FURI_LOG_T(TAG, "on_exit");
    furi_assert(context);
    GasSensorApp* gsa = context;
    popup_reset(gsa->popup);
}