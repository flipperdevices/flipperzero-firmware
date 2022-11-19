#ifndef UNITEMP_LM75
#define UNITEMP_LM75

#include "../unitemp.h"
#include "../Sensors.h"
extern const SensorType LM75;
/**
 * @brief Выделение памяти и установка начальных значений датчика LM75
 *
 * @param sensor Указатель на создаваемый датчик
 * @return Истина при успехе
 */
bool unitemp_LM75_alloc(Sensor* sensor, char* args);

/**
 * @brief Инициализации датчика LM75
 *
 * @param sensor Указатель на датчик
 * @return Истина если инициализация упспешная
 */
bool unitemp_LM75_init(Sensor* sensor);

/**
 * @brief Деинициализация датчика
 *
 * @param sensor Указатель на датчик
 */
bool unitemp_LM75_deinit(Sensor* sensor);

/**
 * @brief Обновление значений из датчика
 *
 * @param sensor Указатель на датчик
 * @return Статус обновления
 */
UnitempStatus unitemp_LM75_update(Sensor* sensor);

/**
 * @brief Высвободить память датчика
 *
 * @param sensor Указатель на датчик
 */
bool unitemp_LM75_free(Sensor* sensor);

#endif