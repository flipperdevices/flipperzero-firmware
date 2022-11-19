#ifndef UNITEMP_SINGLE_WIRE
#define UNITEMP_SINGLE_WIRE

#include "../unitemp.h"
#include "../Sensors.h"

//Интерфейс Single Wire
typedef struct {
    //Порт подключения датчика
    const GPIO* gpio;
} SingleWireSensor;

/* Датчики */
extern const SensorType DHT11;
extern const SensorType DHT12_SW;
extern const SensorType DHT21;
extern const SensorType DHT22;
extern const SensorType AM2320_SW;

/**
 * @brief Инициализация датчика
 * 
 * @param sensor Указатель на инициализируемый датчик
 * @return Истина если всё прошло успешно
 */
bool unitemp_singlewire_init(Sensor* sensor);

/**
 * @brief Деинициализация датчика
 * 
 * @param sensor Указатель на инициализируемый датчик
 * @return Истина если всё прошло успешно
 */
bool unitemp_singlewire_deinit(Sensor* sensor);

/**
 * @brief Получение данных с датчика по однопроводному интерфейсу DHTxx и AM2xxx
 * 
 * @param sensor Указатель на датчик
 * @return Статус опроса
 */
UnitempStatus unitemp_singlewire_update(Sensor* sensor);

/**
 * @brief Установить порт датчика
 * 
 * @param sensor Указатель на датчик
 * @param gpio Устанавливаемый порт
 * @return Истина если всё ок
 */
bool unitemp_singlewire_sensorSetGPIO(Sensor* sensor, const GPIO* gpio);

/**
 * @brief Получить порт датчика
 * 
 * @param sensor Указатель на датчик
 * @return Указатель на GPIO
 */
const GPIO* unitemp_singlewire_sensorGetGPIO(Sensor* sensor);

/**
 * @brief Выделение памяти под датчик на линии One Wire
 * 
 * @param sensor Указатель на датчик
 * @param args Указатель на массив с аргументами параметров датчка
 */
bool unitemp_singlewire_alloc(Sensor* sensor, char* args);

/**
 * @brief Высвобождение памяти инстанса датчика
 * 
 * @param sensor Указатель на датчик
 */
bool unitemp_singlewire_free(Sensor* sensor);
#endif