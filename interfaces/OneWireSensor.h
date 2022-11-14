#ifndef UNITEMP_OneWire
#define UNITEMP_OneWire

#include "../unitemp.h"

typedef enum DallasFamilyCode {
    FC_DS18S20 = 0x10,
    FC_DS1822 = 0x22,
    FC_DS18B20 = 0x28,
} DallasFamilyCode;

typedef struct OneWireSensor {
    //Порт подключения датчика
    const GPIO* gpio;
    //Текущий адрес устройства на шине OneWire
    uint64_t addr;
    DallasFamilyCode familyCode;
} OneWireSensor;

/**
 * @brief Выделение памяти для датчика на шине OneWire
 *
 * @param sensor Указатель на датчик
 * @param st Тип датчика
 * @return Истина если всё ок
 */
bool unitemp_OneWire_alloc(void* s, uint16_t* anotherValues);

/**
 * @brief Высвобождение памяти инстанса датчика
 * 
 * @param sensor Указатель на датчик
 */
bool unitemp_OneWire_free(void* sensor);

/**
 * @brief Инициализации датчика на шине One Wire
 *
 * @param sensor Указатель на датчик (тип Sensor)
 * @return Истина если инициализация упспешная
 */
bool unitemp_OneWire_init(void* sensor);

/**
 * @brief Деинициализация датчика
 *
 * @param sensor Указатель на датчик (тип Sensor)
 */
bool unitemp_OneWire_deinit(void* s);

/**
 * @brief Обновить значение с датчка
 * 
 * @param sensor Указатель на датчик (тип Sensor)
 * @return Статус обновления
 */
UnitempStatus unitemp_OneWire_update(void* sensor);

extern const SensorType DS18x2x;
#endif