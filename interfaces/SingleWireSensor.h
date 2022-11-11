#ifndef UNITEMP_SINGLE_WIRE
#define UNITEMP_SINGLE_WIRE

#include "../unitemp.h"

//Интерфейс One Wire
typedef struct {
    //Тип интерфейса подключения датчика
    Interface interface;
    //Порт подключения датчика
    const GPIO* gpio;
    //Время последнего опроса
    uint32_t lastPollingTime;
    //Последнее успешное значение температуры
    float lastTemp;
    //Последнее успешное значение влажности
    float lastHum;
} SingleWireSensor;

/**
 * @brief Инициализация датчика
 * 
 * @param sensor Указатель на инициализируемый датчик
 * @return Истина если всё прошло успешно
 */
bool unitemp_oneWire_sensorInit(void* sensor);

/**
 * @brief Деинициализация датчика
 * 
 * @param sensor Указатель на инициализируемый датчик
 * @return Истина если всё прошло успешно
 */
bool unitemp_oneWire_sensorDeInit(void* sensor);

/**
 * @brief Получение данных с датчика по однопроводному интерфейсу DHTxx и AM2xxx
 * 
 * @param sensor Указатель на датчик
 * @return Статус опроса
 */
UnitempStatus unitemp_oneWire_updateData(void* sensor);

/**
 * @brief Установить порт датчика
 * 
 * @param sensor Указатель на датчик
 * @param gpio Устанавливаемый порт
 * @return Истина если всё ок
 */
bool unitemp_oneWire_sensorSetGPIO(Sensor* sensor, const GPIO* gpio);

/**
 * @brief Получить порт датчика
 * 
 * @param sensor Указатель на датчик
 * @return Указатель на GPIO
 */
const GPIO* unitemp_oneWire_sensorGetGPIO(Sensor* sensor);

/**
 * @brief Выделение памяти под датчик на линии One Wire
 * 
 * @param sensor Указатель на датчик
 * @param st Тип датчика
 */
bool unitemp_oneWire_sensorAlloc(Sensor* sensor, SensorType st, uint16_t* anotherValues);
#endif