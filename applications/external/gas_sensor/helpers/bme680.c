/**
 * @file  : bme680.c
 * @brief : BME680 Air Sensor Module Implementation
 *
 * Code file containing implementations for the BME680 module.
 * The interface uses the Bosch open source library in "BME68x-Sensor-API-<version>"
 * and provides read and write callback functions for the furi_hal_i2c_* interface
 * used in the FlipperZero firmware.
 *
 * @author Ian S. Cohee
 */
#include "bme680.h"
#include "BME68x_Sensor_API_448/bme68x.h"
#include "../gas_sensor_i.h"

#include <furi_hal_power.h>
#include <stdlib.h>

#define TAG "bme680"

Bme680* bme680_alloc() {
    Bme680* bme680 = malloc(sizeof(Bme680));
    furi_check(bme680);

    bme680->dev = malloc(sizeof(Bme68xDev));
    furi_check(bme680->dev);

    bme680->conf = malloc(sizeof(Bme68xConf));
    furi_check(bme680->conf);

    bme680->heatr_conf = malloc(sizeof(Bme68xHeatrConf));
    furi_check(bme680->heatr_conf);

    bme680->data = malloc(sizeof(Bme68xData));
    furi_check(bme680->data);

    bme680->i2c = &furi_hal_i2c_handle_external;
    bme680->i2c_addr = BME680_I2C_ADDR;
    bme680->i2c_addr_read = ((bme680->i2c_addr << 1) | 0x01);
    bme680->i2c_addr_write = ((bme680->i2c_addr << 1) | 0x00);

    return bme680;
}

void bme680_init(Bme680* bme680) {
    furi_assert(bme680);
    uint8_t chip_id = bme680_get_chip_id(bme680);
    bme680->chip_id = 0x0D;
    if(chip_id != BME68X_CHIP_ID) {
        FURI_LOG_W(
            TAG,
            "reported chip id does not match expected id (0x%02X); leaving chip id unset (0x%02X)",
            BME68X_CHIP_ID,
            bme680->chip_id);
    } else {
        FURI_LOG_D(TAG, "bme680 chip id: 0x%02X", chip_id);
        bme680->chip_id = chip_id;
    }
    if(bme680_config(bme680) == false) {
        FURI_LOG_E(TAG, "failed to configure bme680 device");
    }
}

void bme680_free(Bme680* bme680) {
    furi_assert(bme680);
    free(bme680->data);
    free(bme680->heatr_conf);
    free(bme680->conf);
    free(bme680->dev);
    free(bme680);
}

BME68X_INTF_RET_TYPE
bme680_read_i2c_mem(uint8_t reg_addr, uint8_t* reg_data, uint32_t len, void* intf_ptr) {
    furi_assert(intf_ptr);
    Bme680* bme680 = intf_ptr;
    if(bme680_read_mem(bme680, reg_addr, reg_data, len) != true) {
        return -1;
    }
    return 0;
}

BME68X_INTF_RET_TYPE
bme680_write_i2c_mem(uint8_t reg_addr, const uint8_t* reg_data, uint32_t len, void* intf_ptr) {
    furi_assert(intf_ptr);
    Bme680* bme680 = intf_ptr;
    if(bme680_write_mem(bme680, reg_addr, reg_data, len) != true) {
        return -1;
    }
    return 0;
}

void bme680_furi_delay_us(uint32_t period, void* intf_ptr) {
    furi_assert(intf_ptr);
    furi_delay_us(period);
}

bool bme680_config(Bme680* bme680) {
    furi_assert(bme680);
    int8_t rslt = 0;

    Bme68xDev* device = bme680->dev;
    Bme68xConf* conf = bme680->conf;
    Bme68xHeatrConf* heatr_conf = bme680->heatr_conf;

    FURI_LOG_D(TAG, "configuring bme680 device");
    device->read = bme680_read_i2c_mem;
    device->write = bme680_write_i2c_mem;
    device->intf = BME68X_I2C_INTF;
    device->intf_ptr = bme680;
    device->amb_temp = 25;
    device->delay_us = bme680_furi_delay_us;
    rslt = bme68x_init(device);
    if(rslt != 0) {
        FURI_LOG_E(TAG, "config: failed to init bme68x device, return: %d", rslt);
        return false;
    }

    FURI_LOG_D(TAG, "configuring filters and oversampling");
    conf->filter = BME68X_FILTER_OFF;
    conf->odr = BME68X_ODR_NONE;
    conf->os_hum = BME68X_OS_16X;
    conf->os_pres = BME68X_OS_1X;
    conf->os_temp = BME68X_OS_2X;
    rslt = bme68x_set_conf(conf, device);
    if(rslt != 0) {
        FURI_LOG_E(TAG, "failed to set conf: %d", rslt);
        return false;
    }

    FURI_LOG_D(TAG, "configuring heater");
    heatr_conf->enable = BME68X_ENABLE;
    heatr_conf->heatr_temp = 300;
    heatr_conf->heatr_dur = 100;
    rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, heatr_conf, device);
    if(rslt != 0) {
        FURI_LOG_E(TAG, "failed to set heatr conf: %d", rslt);
        return false;
    }

    FURI_LOG_D(TAG, "bme680 configured");
    return true;
}

uint8_t bme680_get_chip_id(Bme680* bme680) {
    furi_assert(bme680);
    uint8_t data = 0;
    bool status = bme680_read_mem(bme680, BME68X_REG_CHIP_ID, &data, 1);
    if(status == false) {
        FURI_LOG_E(TAG, "failed to read from 0x%02X", data);
        return 0x00;
    }
    return data;
}

bool bme680_forced_mode_get_data(Bme680* bme680) {
    furi_assert(bme680);
    bool status = false;

    if(bme68x_set_op_mode(BME68X_FORCED_MODE, bme680->dev) != 0) {
        FURI_LOG_E(TAG, "failed to set bme680 into forced mode");
        return status;
    }

    uint32_t del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, bme680->conf, bme680->dev) +
                          (bme680_heatr_conf_heatr_dur(bme680) * 1000);
    bme680_dev_delay_us(bme680, del_period);

    if(bme68x_get_data(BME68X_FORCED_MODE, bme680->data, &bme680->num_data, bme680->dev) != 0) {
        FURI_LOG_E(TAG, "failed to get data :(");
        return status;
    }

    status = true;
    return status;
}

uint8_t bme680_data_num_data(Bme680* bme680) {
    furi_assert(bme680);
    return bme680->num_data;
}

void bme680_dev_delay_us(Bme680* bme680, uint32_t period) {
    furi_assert(bme680);
    bme680->dev->delay_us(period, bme680);
}

uint16_t bme680_heatr_conf_heatr_dur(Bme680* bme680) {
    furi_assert(bme680);
    return bme680->heatr_conf->heatr_dur;
}

float bme680_data_temperature(Bme680* bme680) {
    furi_assert(bme680);
    return bme680->data->temperature;
}

float bme680_data_humidity(Bme680* bme680) {
    furi_assert(bme680);
    return bme680->data->humidity;
}

float bme680_data_pressure(Bme680* bme680) {
    furi_assert(bme680);
    return bme680->data->pressure;
}

uint8_t bme680_data_status(Bme680* bme680) {
    furi_assert(bme680);
    return bme680->data->status;
}

uint8_t bme680_data_meas_index(Bme680* bme680) {
    furi_assert(bme680);
    return bme680->data->meas_index;
}

bool bme680_write_mem(Bme680* bme680, uint8_t reg, const uint8_t* data, uint32_t data_len) {
    furi_assert(bme680);
    furi_assert(data);
    bool status = false;

    if(furi_hal_power_is_otg_enabled() != true) {
        furi_hal_power_enable_otg();
    }

    furi_hal_i2c_acquire(bme680->i2c);
    if(furi_hal_i2c_is_device_ready(bme680->i2c, bme680->i2c_addr_write, 10) == true) {
        status =
            furi_hal_i2c_write_mem(bme680->i2c, bme680->i2c_addr_write, reg, data, data_len, 10);
        if(status != true) {
            FURI_LOG_E(TAG, "write_mem: failed to write to register 0x%02X", reg);
        }
    } else {
        FURI_LOG_E(TAG, "write_mem: device not ready");
        status = false;
    }
    furi_hal_i2c_release(bme680->i2c);
    return status;
}

bool bme680_read_mem(Bme680* bme680, uint8_t reg, uint8_t* data, uint32_t data_len) {
    furi_assert(bme680);
    furi_assert(data);
    bool status = false;

    if(furi_hal_power_is_otg_enabled() != true) {
        furi_hal_power_enable_otg();
    }

    furi_hal_i2c_acquire(bme680->i2c);
    if(furi_hal_i2c_is_device_ready(bme680->i2c, bme680->i2c_addr_read, 10) == true) {
        status =
            furi_hal_i2c_read_mem(bme680->i2c, bme680->i2c_addr_read, reg, data, data_len, 10);
        if(status != true) {
            FURI_LOG_E(TAG, "read_mem: failed to read memory at 0x%02X", reg);
        }
    } else {
        FURI_LOG_E(TAG, "read_mem: device not ready");
        status = false;
    }

    furi_hal_i2c_release(bme680->i2c);
    return status;
}