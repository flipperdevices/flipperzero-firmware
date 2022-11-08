#include "I2CSensor.h"

static uint8_t readReg(I2CSensor* i2c_sensor, uint8_t reg) {
    //Блокировка шины
    furi_hal_i2c_acquire(i2c_sensor->i2c);
    uint8_t buff[1];
    furi_hal_i2c_read_mem(i2c_sensor->i2c, i2c_sensor->currentI2CAdr << 1, reg, buff, 1, 0xFF);
    furi_hal_i2c_release(i2c_sensor->i2c);
    return buff[0];
}
static void writeReg(I2CSensor* i2c_sensor, uint8_t reg, uint8_t value) {
    //Блокировка шины
    furi_hal_i2c_acquire(i2c_sensor->i2c);
    uint8_t buff[1] = {value};
    furi_hal_i2c_write_mem(i2c_sensor->i2c, i2c_sensor->currentI2CAdr << 1, reg, buff, 1, 0xFF);
    furi_hal_i2c_release(i2c_sensor->i2c);
}

bool unitemp_I2C_sensorInit(void* sensor) {
    I2CSensor* i2c_sensor = (I2CSensor*)((Sensor*)sensor)->instance;
    //BMP280
    if(((Sensor*)sensor)->type == BMP280) {
        //Перезагрузка
        writeReg(i2c_sensor, 0xE0, 0xB6);
        //Чтение ID датчика
        uint8_t id = readReg(i2c_sensor, 0xD0);
        if(id != 0x58) {
            ((Sensor*)sensor)->status = UT_ERROR;
            return false;
        }

        //TODO: Дальнейшая инициализация

        ((Sensor*)sensor)->status = UT_OK;
        return true;
    }
    return false;
}

bool unitemp_I2C_sensorDeInit(void* sensor) {
    //TODO датчик в спячку, очистить память
    UNUSED(sensor);
    return true;
}

UnitempStatus unitemp_I2C_updateData(void* sensor) {
    if(((Sensor*)sensor)->status == UT_ERROR || ((Sensor*)sensor)->status == UT_TIMEOUT) {
        if(((Sensor*)sensor)->initializer(sensor) != true) return UT_ERROR;
    }
    //I2CSensor* i2c_sensor = (I2CSensor*)((Sensor*)sensor)->instance;
    return UT_OK;
}

bool unitemp_I2C_sensorAlloc(Sensor* sensor, SensorType st, uint16_t* anotherValues) {
    I2CSensor* instance = malloc(sizeof(I2CSensor));
    instance->interface = I2C;
    instance->i2c = &furi_hal_i2c_handle_external;

    instance->lastPollingTime = 0xFFFFFFFF;

    sensor->initializer = unitemp_I2C_sensorInit;
    sensor->deinitializer = unitemp_I2C_sensorDeInit;
    sensor->updater = unitemp_I2C_updateData;

    sensor->instance = instance;
    sensor->type = st;

    //Настройки для BMP280
    if(st == BMP280) {
        instance->minI2CAdr = 0x76;
        instance->maxI2CAdr = 0x77;
    }

    if(anotherValues[0] >= instance->minI2CAdr && anotherValues[0] <= instance->maxI2CAdr) {
        instance->currentI2CAdr = anotherValues[0];
    } else {
        instance->currentI2CAdr = instance->minI2CAdr;
    }
    return true;
}