#include "LM75.h"
#include "../interfaces/I2CSensor.h"

#define LM75_REG_TEMP 0x00
#define LM75_REG_CONFIG 0x01
#define LM75_REG_THYST 0x02
#define LM75_REG_TOS 0x03

#define LM75_CONFIG_SHUTDOWN 0b00000001
#define LM75_CONFIG_INTERRUPT 0b00000010
#define LM75_CONFIG_OSPOLARITY_HIGH 0b00000100
#define LM75_CONFIG_FAULTQUEUE_1 0b00000000
#define LM75_CONFIG_FAULTQUEUE_2 0b00001000
#define LM75_CONFIG_FAULTQUEUE_4 0b00010000
#define LM75_CONFIG_FAULTQUEUE_6 0b00011000

const SensorType LM75 = {
    .typename = "LM75",
    .interface = &I2C,
    .pollingInterval = 500,
    .allocator = unitemp_LM75_alloc,
    .mem_releaser = unitemp_LM75_free,
    .initializer = unitemp_LM75_init,
    .deinitializer = unitemp_LM75_deinit,
    .updater = unitemp_LM75_update};

bool unitemp_LM75_alloc(Sensor* sensor, char* args) {
    UNUSED(args);
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;

    //Адреса на шине I2C (7 бит)
    i2c_sensor->minI2CAdr = 0b1001000;
    i2c_sensor->maxI2CAdr = 0b1001111;
    return true;
}

bool unitemp_LM75_free(Sensor* sensor) {
    //Нечего высвобождать, так как ничего не было выделено
    UNUSED(sensor);
    return true;
}

bool unitemp_LM75_init(Sensor* sensor) {
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;

    //Выход если не удалось записать значение в датчик
    if(!unitemp_i2c_writeReg(i2c_sensor, LM75_REG_CONFIG, LM75_CONFIG_FAULTQUEUE_1)) return false;

    return true;
}

bool unitemp_LM75_deinit(Sensor* sensor) {
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;
    if(!unitemp_i2c_writeReg(
           i2c_sensor, LM75_REG_CONFIG, LM75_CONFIG_FAULTQUEUE_1 | LM75_CONFIG_SHUTDOWN))
        return false;
    return true;
}

UnitempStatus unitemp_LM75_update(Sensor* sensor) {
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;

    uint8_t buff[2];
    if(!unitemp_i2c_readRegArray(i2c_sensor, LM75_REG_TEMP, 2, buff)) return UT_TIMEOUT;
    int16_t raw = ((((uint16_t)buff[0] << 8) | buff[1]) >> 7);

    if(FURI_BIT(raw, 8)) {
        FURI_BIT_CLEAR(raw, 8);
        raw = (int8_t)raw;
    }
    sensor->temp = (float)raw / 2.0f;

    return UT_OK;
}
