#ifndef UNITEMP_I2C
#define UNITEMP_I2C

#include "../unitemp.h"

#include <furi_hal_i2c.h>

typedef struct I2CSensor {
    //Тип интерфейса подключения датчика
    Interface interface;
    //Время последнего опроса
    uint32_t lastPollingTime;
    //Последнее успешное значение температуры
    float lastTemp;
    //Последнее успешное значение влажности
    float lastHum;
    //Указатель на интерфейс I2C
    FuriHalI2cBusHandle* i2c;
    //Минимальный адрес устройства на шине I2C
    uint8_t minI2CAdr;
    //Максимальный адрес устройства на шине I2C
    uint8_t maxI2CAdr;
    //Текущий адрес устройства на шине I2C
    uint8_t currentI2CAdr;

} I2CSensor;

/**
 * @brief Функция инициализации датчика на шине I2C
 * 
 * @param sensor Указатель на датчик
 * @return Истина если датчик на шине
 */
bool unitemp_I2C_sensorInit(void* sensor);
/**
 * @brief Выделение памяти для датчика на шине I2C
 * 
 * @param sensor Указатель на датчик
 * @param st Тип датчика
 * @return Истина если всё ок
 */
bool unitemp_I2C_sensorAlloc(Sensor* sensor, SensorType st, uint16_t* anotherValues);

#endif