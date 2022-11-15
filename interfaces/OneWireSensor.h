#ifndef UNITEMP_OneWire
#define UNITEMP_OneWire

#include "../unitemp.h"

//Коды семейства устройств
typedef enum DallasFamilyCode {
    FC_DS18S20 = 0x10,
    FC_DS1822 = 0x22,
    FC_DS18B20 = 0x28,
} DallasFamilyCode;

//Режим питания датчка
typedef enum PowerMode {
    PWR_PASSIVE, //Питание от линии данных
    PWR_ACTIVE //Питание от источника питания
} PowerMode;

typedef struct {
    //Порт подключения датчика
    const GPIO* gpio;
    //Количество устройств на шине
    //Обновляется при ручном добавлении датчика на эту шину
    uint8_t device_count;
    //Режим питания датчиков на шине
    PowerMode powerMode;
} OneWireBus;

typedef struct OneWireSensor {
    //Указатель на шину OneWire
    OneWireBus* bus;
    //Текущий адрес устройства на шине OneWire
    uint8_t deviceID[8];
    //Код семейства устройств
    DallasFamilyCode familyCode;

} OneWireSensor;

/**
 * @brief Выделение памяти для датчика на шине OneWire
 *
 * @param sensor Указатель на датчик
 * @param st Тип датчика
 * @return Истина если всё ок
 */
bool unitemp_OneWire_sensor_alloc(void* s, uint8_t* anotherValues);

/**
 * @brief Высвобождение памяти инстанса датчика
 * 
 * @param sensor Указатель на датчик
 */
bool unitemp_OneWire_sensor_free(void* sensor);

/**
 * @brief Инициализации датчика на шине One Wire
 *
 * @param sensor Указатель на датчик (тип Sensor)
 * @return Истина если инициализация упспешная
 */
bool unitemp_OneWire_sensor_init(void* sensor);

/**
 * @brief Деинициализация датчика
 *
 * @param sensor Указатель на датчик (тип Sensor)
 */
bool unitemp_OneWire_sensor_deinit(void* s);

/**
 * @brief Обновить значение с датчка
 * 
 * @param sensor Указатель на датчик (тип Sensor)
 * @return Статус обновления
 */
UnitempStatus unitemp_OneWire_sensor_update(void* sensor);

/**
 * @brief Запуск общения с датчиком 
 * 
 * @param sensor Указатель на датчик 
 * @return Истина если датчик отозвался
 */
bool unitemp_oneWire_sensor_start(OneWireSensor* instance);

extern const SensorType DS18x2x;
#endif