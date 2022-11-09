#include "SensorsDriver.h"
#include "BMP280.h"

#define TEMP_CAL_START_ADDR 0x88
#define PRESS_CAL_START_ADDR 0x8E

static double bmp280_compensate_T_double(I2CSensor* i2c_sensor, int32_t adc_T) {
    BMP280_instance* bmp280_instance = (BMP280_instance*)i2c_sensor->sensorInstance;
    double var1, var2, T;
    var1 = (((double)adc_T) / (double)16384.0 -
            ((double)bmp280_instance->temp_cal.dig_T1) / (double)1024.0) *
           ((double)bmp280_instance->temp_cal.dig_T2);
    var2 = ((((double)adc_T) / (double)131072.0 -
             ((double)bmp280_instance->temp_cal.dig_T1) / (double)8192.0) *
            (((double)adc_T) / (double)131072.0 -
             ((double)bmp280_instance->temp_cal.dig_T1) / (double)8192.0)) *
           ((double)bmp280_instance->temp_cal.dig_T3);
    T = (var1 + var2) / (double)5120.0;
    return T;
}

static bool readCalValues(I2CSensor* i2c_sensor) {
    BMP280_instance* bmp280_instance = (BMP280_instance*)i2c_sensor->sensorInstance;
    if(!readRegArray(i2c_sensor, TEMP_CAL_START_ADDR, 6, (uint8_t*)&bmp280_instance->temp_cal))
        return false;

    if(!readRegArray(i2c_sensor, PRESS_CAL_START_ADDR, 18, (uint8_t*)&bmp280_instance->press_cal))
        return false;
    FURI_LOG_D(
        APP_NAME,
        "Sensor BMP280 (0x%02X): T1-3: %d, %d, %d; P1-9: %d, %d, %d, %d, %d, %d, %d, %d, %d",
        i2c_sensor->currentI2CAdr,
        bmp280_instance->temp_cal.dig_T1,
        bmp280_instance->temp_cal.dig_T2,
        bmp280_instance->temp_cal.dig_T3,
        bmp280_instance->press_cal.dig_P1,
        bmp280_instance->press_cal.dig_P2,
        bmp280_instance->press_cal.dig_P3,
        bmp280_instance->press_cal.dig_P4,
        bmp280_instance->press_cal.dig_P5,
        bmp280_instance->press_cal.dig_P6,
        bmp280_instance->press_cal.dig_P7,
        bmp280_instance->press_cal.dig_P8,
        bmp280_instance->press_cal.dig_P9);
    return true;
}

bool BMP280_init(I2CSensor* i2c_sensor) {
    //Перезагрузка
    writeReg(i2c_sensor, 0xE0, 0xB6);
    //Чтение ID датчика
    if(readReg(i2c_sensor, 0xD0) != 0x58) {
        return false;
    }
    i2c_sensor->sensorInstance = malloc(sizeof(BMP280_instance));
    //Чтение калибровочных значений
    if(!readCalValues(i2c_sensor)) {
        return false;
    }

    writeReg(i2c_sensor, 0xF4, 0b01010111);
    writeReg(i2c_sensor, 0xF5, 0b10110100);

    return true;
}

bool BMP280_updateData(Sensor* sensor) {
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;
    if(furi_get_tick() - i2c_sensor->lastPollingTime < 500) {
        sensor->status = UT_EARLYPOOL;
        return false;
    }
    i2c_sensor->lastPollingTime = furi_get_tick();

    while(readReg(i2c_sensor, 0xF3) & 0b00001001) {
        if(furi_get_tick() - i2c_sensor->lastPollingTime > 100) {
            sensor->status = UT_TIMEOUT;
            return false;
        }
    }

    uint8_t buff[3];
    if(!readRegArray(i2c_sensor, 0xFA, 3, buff)) return false;
    int32_t adc_T = ((int32_t)buff[2] << 12) | ((int32_t)buff[1] << 4) | ((int32_t)buff[2] >> 4);
    sensor->temp = bmp280_compensate_T_double(i2c_sensor, adc_T);
    return true;
}