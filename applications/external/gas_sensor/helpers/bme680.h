/**
 * @file  : bme680.h
 * @brief : BME680 Air Sensor Module Header
 *
 * Header file containing declarations for the BME680 module.
 * The interface uses the Bosch open source library in "BME68x-Sensor-API-<version>"
 * and provides read and write callback functions for the furi_hal_i2c_* interface
 * used in the FlipperZero firmware.
 *
 * @author Ian S. Cohee
 */
#pragma once

#include <furi.h>
#include <furi_hal_i2c.h>

#include "BME68x_Sensor_API_448/bme68x.h"

/**
 * @brief Typedefs of BME68x-Sensor-API definitions
*/
typedef struct bme68x_dev Bme68xDev;
typedef struct bme68x_conf Bme68xConf;
typedef struct bme68x_heatr_conf Bme68xHeatrConf;
typedef struct bme68x_data Bme68xData;

#define BME680_I2C_ADDR UINT8_C(0x77)

/**
 * @brief Bme680 instance definition
*/
typedef struct Bme680 {
    uint8_t chip_id;
    FuriHalI2cBusHandle* i2c;
    Bme68xDev* dev;
    Bme68xConf* conf;
    Bme68xHeatrConf* heatr_conf;
    Bme68xData* data;
    uint8_t num_data;

    uint8_t i2c_addr;
    uint8_t i2c_addr_read;
    uint8_t i2c_addr_write;
} Bme680;

/**
 * @brief Allocate a Bme680 instance.
 *
 * @return Bme680 instance.
 */
Bme680* bme680_alloc();

/**
 * @brief Check chip ID and configure Bme680.
 *
 * @return Void.
 */
void bme680_init(Bme680* bme680);

/**
 * @brief Free a Bme680 instance.
 *
 * @param[in]  bme680 : Pointer to Bme680 instance
 *
 * @return void.
 */
void bme680_free(Bme680* bme680);

/**
 * @brief Configure Bme680 according to the datasheet example.
 * 
 * @param[in]  bme680 : Pointer to Bme680 instance
 *
 * @return true on success, false on failure.
 */
bool bme680_config(Bme680* bme680);

/**
 * @brief Get the chip ID from the sensor.
 * 
 * @param[in]  bme680 : Pointer to Bme680 instance
 *
 * @return The chip id returned from sensor memory.
 */
uint8_t bme680_get_chip_id(Bme680* bme680);

/**
 * @brief Set the sensor into Forced Mode and read data into 
 * its internal registers
 * 
 * @param[in]  bme680 : Pointer to Bme680 instance
 *
 * @return True on success, false on failure.
 */
bool bme680_forced_mode_get_data(Bme680* bme680);

/**
 * @brief Get the number of data available after sampling.
 * Not generally needed to read data.
 * 
 * @param[in]  bme680 : Pointer to Bme680 instance
 *
 * @return The count of data available.
 */
uint8_t bme680_data_num_data(Bme680* bme680);

/**
 * @brief Wrapper around furi_delay_us and passed the BME68x-Sensor-API 
 * library to be used as a timing callback.
 * 
 * @param[in]  bme680 : Pointer to Bme680 instance
 * @param[in]  period : Delay in useconds
 *
 * @return Void.
 */
void bme680_dev_delay_us(Bme680* bme680, uint32_t period);

/**
 * @brief Get heatr_conf - heater timing and temperature configuration.
 * 
 * @param[in]  bme680 : Pointer to Bme680 instance
 *
 * @return 16-bit heatr_conf data.
 */
uint16_t bme680_heatr_conf_heatr_dur(Bme680* bme680);

/**
 * @brief Get temperature data from instance.
 * 
 * @param[in]  bme680 : Pointer to Bme680 instance
 *
 * @return Temperature in Celcius.
 */
float bme680_data_temperature(Bme680* bme680);

/**
 * @brief Get humidity data from instance.
 * 
 * @param[in]  bme680 : Pointer to Bme680 instance
 *
 * @return Humidity in relative percent.
 */
float bme680_data_humidity(Bme680* bme680);

/**
 * @brief Get pressure data from instance.
 * 
 * @param[in]  bme680 : Pointer to Bme680 instance
 *
 * @return Pressure in pascals.
 */
float bme680_data_pressure(Bme680* bme680);

/**
 * @brief Get data status.
 * 
 * @param[in]  bme680 : Pointer to Bme680 instance
 *
 * @return Data Status.
 */
uint8_t bme680_data_status(Bme680* bme680);

/**
 * @brief Get measurement index.
 * 
 * @param[in]  bme680 : Pointer to Bme680 instance
 *
 * @return Measurement index.
 */
uint8_t bme680_data_meas_index(Bme680* bme680);

/**
 * @brief Write data to a register.
 * 
 * @param[in]  bme680   : Pointer to Bme680 instance
 * @param[in]  reg      : Target register
 * @param[in]  data     : Sequence of data bytes to write
 * @param[in]  data_len : Number of bytes to write
 *
 * @return True on success, false on failure.
 */
bool bme680_write_mem(Bme680* bme680, uint8_t reg, const uint8_t* data, uint32_t data_len);

/**
 * @brief Read data from a register.
 * 
 * @param[in]  bme680   : Pointer to Bme680 instance
 * @param[in]  reg      : Target register
 * @param[in]  data     : Pointer to sequence of bytes to contain data
 * @param[in]  data_len : Number of bytes to read into data
 *
 * @return True on success, false on failure.
 */
bool bme680_read_mem(Bme680* bme680, uint8_t reg, uint8_t* data, uint32_t data_len);

/* Implementations for the Bosch BME68X API */

/*!
 * @brief Bus communication function pointer which should be mapped to
 * the platform specific read functions of the user
 *
 * @param[in]     reg_addr : 8bit register address of the sensor
 * @param[out]    reg_data : Data from the specified address
 * @param[in]     length   : Length of the reg_data array
 * @param[in,out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                           for interface related callbacks
 * @retval 0 for Success
 * @retval Non-zero for Failure
 */
BME68X_INTF_RET_TYPE
bme680_read_i2c_mem(uint8_t reg_addr, uint8_t* reg_data, uint32_t len, void* intf_ptr);

/*!
 * @brief Bus communication function pointer which should be mapped to
 * the platform specific write functions of the user
 *
 * @param[in]     reg_addr : 8bit register address of the sensor
 * @param[out]    reg_data : Data to the specified address
 * @param[in]     length   : Length of the reg_data array
 * @param[in,out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                           for interface related callbacks
 * @retval 0 for Success
 * @retval Non-zero for Failure
 *
 */
BME68X_INTF_RET_TYPE
bme680_write_i2c_mem(uint8_t reg_addr, const uint8_t* reg_data, uint32_t len, void* intf_ptr);