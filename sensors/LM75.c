#include "LM75.h"

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

bool unitemp_LM75_alloc(Sensor* sensor) {
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;
    //Функции работы с датчиком
    sensor->initializer = unitemp_LM75_init;
    sensor->deinitializer = unitemp_LM75_deinit;
    sensor->updater = unitemp_LM75_update;
    sensor->pollingInterval = 1000;

    //Адреса на шине I2C (7 бит)
    i2c_sensor->minI2CAdr = 0b1001000;
    i2c_sensor->maxI2CAdr = 0b1001111;
    return true;
}

bool unitemp_LM75_init(void* s) {
    Sensor* sensor = (Sensor*)s;
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;

    //Выход если не удалось записать значение в датчик
    if(!writeReg(i2c_sensor, LM75_REG_CONFIG, LM75_CONFIG_FAULTQUEUE_1)) return false;
    //TODO: Работа с прерываниями и компаратором

    return true;
}

bool unitemp_LM75_deinit(void* s) {
    Sensor* sensor = (Sensor*)s;
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;
    if(!writeReg(i2c_sensor, LM75_REG_CONFIG, LM75_CONFIG_FAULTQUEUE_1 | LM75_CONFIG_SHUTDOWN))
        return false;
    UNUSED(sensor);
    UNUSED(i2c_sensor);
    return true;
}

UnitempStatus unitemp_LM75_update(void* s) {
    Sensor* sensor = (Sensor*)s;
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;

    uint8_t buff[2];
    if(!readRegArray(i2c_sensor, LM75_REG_TEMP, 2, buff)) return UT_TIMEOUT;
    uint16_t raw =
        ((((uint16_t)buff[0] << 8) | buff[1]) >> 7) * ((buff[1] & 0b10000000) ? -1.0f : 1.0f);
    sensor->temp = (float)raw / 2.0f;

    FURI_LOG_D(APP_NAME, "Sensor %s updated %f", sensor->name, (double)raw / (double)2.0);

    return UT_OK;
}
