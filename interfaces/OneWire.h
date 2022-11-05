#ifndef UNITEMP_ONE_WIRE
#define UNITEMP_ONE_WIRE

#include "../unitemp.h"

//Интерфейс One Wire
typedef struct {
    //Тип интерфейса подключения датчика
    Interface interface;
    //Порт подключения датчика
    const GPIO* gpio;
    //Время последнего опроса
    uint32_t lastPollingTime;
} OneWireSensor;

/**
 * @brief Инициализация датчика
 * 
 * @param sensor Указатель на инициализируемый датчик
 * @return Истина если всё прошло успешно
 */
bool unitemp_oneWire_sensorInit(OneWireSensor* sensor);

/**
 * @brief Деинициализация датчика
 * 
 * @param sensor Указатель на инициализируемый датчик
 * @return Истина если всё прошло успешно
 */
bool unitemp_oneWire_sensorDeInit(OneWireSensor* sensor);

/**
 * @brief Получение данных с датчика по однопроводному интерфейсу DHTxx и AM2xxx
 * 
 * @param sensor Указатель на датчик
 * @return Статус опроса
 */
UnitempStatus unitemp_oneWire_getData(Sensor* sensor);

#endif