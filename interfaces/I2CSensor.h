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
    //Указатель на собственный экземпляр датчика
    void* sensorInstance;

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

/**
 * @brief Прочитать значение регистра reg
 * 
 * @param i2c_sensor Указатель на датчик
 * @param reg Номер регистра
 * @return Значение регистра
 */
uint8_t readReg(I2CSensor* i2c_sensor, uint8_t reg);

/**
 * @brief Прочитать масссив значений из памяти 
 * 
 * @param i2c_sensor 
 * @param startReg 
 * @param len 
 * @param data 
 * @return Истина если устройство вернуло данные
 */
bool readRegArray(I2CSensor* i2c_sensor, uint8_t startReg, uint8_t len, uint8_t* data);

/**
 * @brief Записать значение в регистр
 * 
 * @param i2c_sensor Указатель на датчик
 * @param reg Номер регистра
 * @param value Значение для записи
 */
void writeReg(I2CSensor* i2c_sensor, uint8_t reg, uint8_t value);

#endif