#pragma once

#include <furi.h>
#include <furi_hal_i2c.h>

#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/popup.h>
#include <notification/notification_messages.h>

#include "helpers/bme680.h"
#include "helpers/sgp30.h"

#define GAS_SENSOR_TEXT_BUFFER_SIZE UINT8_C(128)

/**
 * @brief Cache for readings from various sensors.
*/
typedef struct GasSensorMeasurementRegs {
    float temperature;
    float humidity;
    float pressure;
    uint16_t co2;
    uint16_t tvoc;
} GasSensorMeasurementRegs;

/**
 * @brief Application instance definition.
*/
typedef struct GasSensorApp {
    Gui* gui;
    NotificationApp* notifications;
    Popup* popup;
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    GasSensorMeasurementRegs* measurement_regs;
    char* text_buffer;

    Bme680* bme680;
    Sgp30* sgp30;

    uint8_t sensor_count;
    uint32_t last_update_tick;
} GasSensorApp;

/**
 * @brief Enum of Views used by application
*/
typedef enum {
    GasSensorViewSensorInfo,

    // Keep this last
    GasSensorViewIdNum
} GasSensorViewId;

/**
 * @brief Allocate GasSensorApp instance.
 *
 * @return GasSensorApp pointer.
 */
GasSensorApp* gas_sensor_app_alloc();

/**
 * @brief Free GasSensorApp.
 *
 * @param[in]  gsa : GasSensorApp pointer.
 *
 * @return Void.
 */
void gas_sensor_app_free(GasSensorApp* gsa);

/**
 * @brief Scan I2C address and log addresses that respond.
 *
 * @param[in]  gsa : GasSensorApp pointer.
 *
 * @return Void.
 */
void gas_sensor_i2c_scan_bus(GasSensorApp*);

/**
 * @brief Update internal registers with fresh data from sensors.
 *
 * @param[in]  gsa : GasSensorApp pointer.
 *
 * @return True on success, false on failure.
 */
bool gas_sensor_update_sensors(GasSensorApp*);

/**
 * @brief Get pointer to Bme680 sensor.
 *
 * @param[in]  gsa : GasSensorApp pointer.
 *
 * @return Pointer to Bme680.
 */
Bme680* gas_sensor_get_bme680(GasSensorApp*);

/**
 * @brief Get sensor count.
 *
 * @param[in]  gsa : GasSensorApp pointer.
 *
 * @return Number of sensors used by the application.
 */
uint8_t gas_sensor_get_sensor_count(GasSensorApp*);

/**
 * @brief Prepare internal text buffer for rendering.
 *
 * @param[in]  gsa : GasSensorApp pointer.
 *
 * @return Void.
 */
void gas_sensor_prepare_text_buffer(GasSensorApp*);

/**
 * @brief Get the internal text buffer.
 *
 * @param[in]  gsa : GasSensorApp pointer.
 *
 * @return Pointer to interal text buffer.
 */
char* gas_sensor_get_text_buffer(GasSensorApp*);

/**
 * @brief Get cached humidity reading.
 *
 * @param[in]  gsa : GasSensorApp pointer.
 *
 * @return Humidity reading in relative percent.
 */
float gas_sensor_get_humidity(GasSensorApp*);

/**
 * @brief Get cached pressure reading.
 *
 * @param[in]  gsa : GasSensorApp pointer.
 *
 * @return Pressure reading in Pa.
 */
float gas_sensor_get_pressure(GasSensorApp*);

/**
 * @brief Get cached temperature reading.
 *
 * @param[in]  gsa : GasSensorApp pointer.
 *
 * @return Temperature reading in Celcius.
 */
float gas_sensor_get_temperature(GasSensorApp*);

/**
 * @brief Get cached CO2 reading.
 *
 * @param[in]  gsa : GasSensorApp pointer.
 *
 * @return CO2 reading in ppm.
 */
uint16_t gas_sensor_get_co2(GasSensorApp* gsa);

/**
 * @brief Get cached TVOC reading.
 *
 * @param[in]  gsa : GasSensorApp pointer.
 *
 * @return TVOC reading in ppb.
 */
uint16_t gas_sensor_get_tvoc(GasSensorApp* gsa);