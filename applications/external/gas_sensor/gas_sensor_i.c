#include "gas_sensor_i.h"
#include "scenes/gas_sensor_scene.h"

#include <furi.h>
#include <furi_hal_i2c.h>
#include <furi_hal_power.h>
#include <stdlib.h>

#define TAG "gas_sensor"

enum SensorArrayIndex {
    SensorArrayIndexBme680,
    SensorArrayIndexMics6814,

    // Keep this last
    SensorArrayIndexNum
};

static bool gas_sensor_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    GasSensorApp* gas_sensor_app = context;
    return scene_manager_handle_custom_event(gas_sensor_app->scene_manager, event);
}

static bool gas_sensor_back_event_callback(void* context) {
    furi_assert(context);
    GasSensorApp* gas_sensor_app = context;
    return scene_manager_handle_back_event(gas_sensor_app->scene_manager);
}

static void gas_sensor_tick_event_callback(void* context) {
    furi_assert(context);
    GasSensorApp* gas_sensor_app = context;
    scene_manager_handle_tick_event(gas_sensor_app->scene_manager);
}

bool gas_sensor_update_regs(GasSensorApp* gsa) {
    furi_assert(gsa);
    FURI_LOG_D(TAG, "getting bme680 data via forced mode");
    if(bme680_forced_mode_get_data(gsa->bme680) == false) {
        FURI_LOG_E(TAG, "forced mode read failed on bme680");
        return false;
    }
    gsa->measurement_regs->humidity = bme680_data_humidity(gsa->bme680);
    gsa->measurement_regs->pressure = bme680_data_pressure(gsa->bme680);
    gsa->measurement_regs->temperature = bme680_data_temperature(gsa->bme680);

    FURI_LOG_D(TAG, "getting sgp30 data");
    Sgp30Readings sgp30_readings;
    if(sgp30_measure_air_quality(gsa->sgp30, &sgp30_readings) == false) {
        FURI_LOG_E(TAG, "failed to read measurements from sgp30");
        return false;
    }

    gsa->measurement_regs->co2 = sgp30_readings.co2;
    gsa->measurement_regs->tvoc = sgp30_readings.tvoc;
    return true;
}

GasSensorApp* gas_sensor_app_alloc() {
    GasSensorApp* gsa = malloc(sizeof(GasSensorApp));
    furi_assert(gsa);

    FURI_LOG_D(TAG, "allocating gui modules and views");
    gsa->gui = furi_record_open(RECORD_GUI);
    gsa->view_dispatcher = view_dispatcher_alloc();
    gsa->scene_manager = scene_manager_alloc(&gas_sensor_scene_handlers, gsa);
    view_dispatcher_enable_queue(gsa->view_dispatcher);
    view_dispatcher_set_event_callback_context(gsa->view_dispatcher, gsa);

    view_dispatcher_set_custom_event_callback(
        gsa->view_dispatcher, gas_sensor_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        gsa->view_dispatcher, gas_sensor_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        gsa->view_dispatcher, gas_sensor_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(gsa->view_dispatcher, gsa->gui, ViewDispatcherTypeFullscreen);
    gsa->notifications = furi_record_open(RECORD_NOTIFICATION);

    gsa->popup = popup_alloc();
    view_dispatcher_add_view(
        gsa->view_dispatcher, GasSensorViewSensorInfo, popup_get_view(gsa->popup));

    FURI_LOG_D(TAG, "allocating sensor devices");
    gsa->measurement_regs = malloc(sizeof(GasSensorMeasurementRegs));

    gsa->bme680 = bme680_alloc();
    bme680_init(gsa->bme680);

    gsa->sgp30 = sgp30_alloc();
    sgp30_init(gsa->sgp30);

    gsa->sensor_count = SensorArrayIndexNum;

    gsa->text_buffer = (char*)malloc(GAS_SENSOR_TEXT_BUFFER_SIZE);
    furi_assert(gsa->text_buffer);

    scene_manager_next_scene(gsa->scene_manager, GasSensorSceneSensorInfo);
    return gsa;
}

void gas_sensor_app_free(GasSensorApp* gsa) {
    furi_assert(gsa);
    free(gsa->text_buffer);
    sgp30_free(gsa->sgp30);
    bme680_free(gsa->bme680);
    free(gsa->measurement_regs);

    view_dispatcher_remove_view(gsa->view_dispatcher, GasSensorViewSensorInfo);
    popup_free(gsa->popup);

    view_dispatcher_free(gsa->view_dispatcher);
    scene_manager_free(gsa->scene_manager);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    free(gsa);
}

void gas_sensor_i2c_scan_bus(GasSensorApp* gsa) {
    UNUSED(gsa);
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    for(uint8_t addr = 0; addr < 0xFF; ++addr) {
        if(furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, addr, 10) == true) {
            FURI_LOG_D(TAG, "bus_scan: device found at ID 0x%02X", addr);
        }
    }
    if(furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, 0xFF, 10) == true) {
        FURI_LOG_D(TAG, "bus_scan: device found at ID 0x%02X", 0xFF);
    }
    FURI_LOG_D(TAG, "bus_scan: complete");
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
}

Bme680* gas_sensor_bme680(GasSensorApp* gsa) {
    furi_assert(gsa);
    return gsa->bme680;
}

uint8_t gas_sensor_get_sensor_count(GasSensorApp* gsa) {
    furi_assert(gsa);
    return gsa->sensor_count;
}

bool gas_sensor_update_sensors(GasSensorApp* gsa) {
    furi_assert(gsa);
    if(bme680_forced_mode_get_data(gsa->bme680) == false) {
        return false;
    }
    if(gas_sensor_update_regs(gsa) == false) {
        return false;
    }
    gas_sensor_prepare_text_buffer(gsa);
    gsa->last_update_tick = furi_get_tick();
    return true;
}

void gas_sensor_prepare_text_buffer(GasSensorApp* gsa) {
    furi_assert(gsa);
    float t = gsa->measurement_regs->temperature;
    float h = gsa->measurement_regs->humidity;
    float t_f = (1.8 * t) + 32;

    int printed = snprintf(
        gsa->text_buffer,
        GAS_SENSOR_TEXT_BUFFER_SIZE,
        "T:    %d.%.3d C / %d.%.3d F\nH:     %d.%.3d %%\nCO2:    %5u ppm\nTVOC:    %5u ppb",
        (int)t,
        (int)((t - (int)t) * 1000),
        (int)t_f,
        (int)((t_f - (int)t_f) * 1000),
        (int)h,
        (int)((h - (int)h) * 1000),
        gsa->measurement_regs->co2,
        gsa->measurement_regs->tvoc);
    if(printed > GAS_SENSOR_TEXT_BUFFER_SIZE) {
        FURI_LOG_W(
            TAG,
            "text_buffer may be truncated; sprintf returned %d, buffer size is: %u",
            printed,
            GAS_SENSOR_TEXT_BUFFER_SIZE);
    }
}

char* gas_sensor_get_text_buffer(GasSensorApp* gsa) {
    furi_assert(gsa);
    return gsa->text_buffer;
}

float gas_sensor_get_humidity(GasSensorApp* gsa) {
    furi_assert(gsa);
    return gsa->measurement_regs->humidity;
}

float gas_sensor_get_pressure(GasSensorApp* gsa) {
    furi_assert(gsa);
    return gsa->measurement_regs->pressure;
}
float gas_sensor_get_temperature(GasSensorApp* gsa) {
    furi_assert(gsa);
    return gsa->measurement_regs->temperature;
}

uint16_t gas_sensor_get_co2(GasSensorApp* gsa) {
    furi_assert(gsa);
    return gsa->measurement_regs->co2;
}

uint16_t gas_sensor_get_tvoc(GasSensorApp* gsa) {
    furi_assert(gsa);
    return gsa->measurement_regs->tvoc;
}