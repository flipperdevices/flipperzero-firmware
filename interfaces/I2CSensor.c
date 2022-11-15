#include "I2CSensor.h"
#include "../sensors/SensorsDriver.h"

uint8_t readReg(I2CSensor* i2c_sensor, uint8_t reg) {
    //Блокировка шины
    furi_hal_i2c_acquire(i2c_sensor->i2c);
    uint8_t buff[1];
    furi_hal_i2c_read_mem(i2c_sensor->i2c, i2c_sensor->currentI2CAdr << 1, reg, buff, 1, 0xFF);
    furi_hal_i2c_release(i2c_sensor->i2c);
    return buff[0];
}

bool readRegArray(I2CSensor* i2c_sensor, uint8_t startReg, uint8_t len, uint8_t* data) {
    furi_hal_i2c_acquire(i2c_sensor->i2c);
    bool status = furi_hal_i2c_read_mem(
        i2c_sensor->i2c, i2c_sensor->currentI2CAdr << 1, startReg, data, len, 0xFF);
    furi_hal_i2c_release(i2c_sensor->i2c);
    return status;
}
bool writeReg(I2CSensor* i2c_sensor, uint8_t reg, uint8_t value) {
    //Блокировка шины
    furi_hal_i2c_acquire(i2c_sensor->i2c);
    uint8_t buff[1] = {value};
    bool status = furi_hal_i2c_write_mem(
        i2c_sensor->i2c, i2c_sensor->currentI2CAdr << 1, reg, buff, 1, 0xFF);
    furi_hal_i2c_release(i2c_sensor->i2c);
    return status;
}

bool unitemp_I2C_sensor_alloc(void* s, uint8_t* anotherValues) {
    Sensor* sensor = (Sensor*)s;
    bool status = false;
    I2CSensor* instance = malloc(sizeof(I2CSensor));
    if(instance == NULL) {
        FURI_LOG_E(APP_NAME, "Sensor %s instance allocation error", sensor->name);
        return false;
    }
    instance->i2c = &furi_hal_i2c_handle_external;
    sensor->instance = instance;

    //Указание функций инициализации, деинициализации и обновления данных, а так же адреса на шине I2C
    status = sensor->type->allocator(sensor, anotherValues);

    //Установка адреса шины I2C
    if(anotherValues[0] >= instance->minI2CAdr && anotherValues[0] <= instance->maxI2CAdr) {
        instance->currentI2CAdr = anotherValues[0];
    } else {
        instance->currentI2CAdr = instance->minI2CAdr;
    }
    return status;
}

bool unitemp_I2C_sensor_free(void* s) {
    Sensor* sensor = (Sensor*)s;
    bool status = sensor->type->mem_releaser(sensor);
    free(sensor->instance);
    return status;
}

UnitempStatus unitemp_I2C_sensor_update(void* s) {
    Sensor* sensor = (Sensor*)s;
    if(sensor->status != UT_OK) {
        sensor->type->initializer(sensor);
    }
    return sensor->type->updater(sensor);
}