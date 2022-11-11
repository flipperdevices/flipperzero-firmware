#ifndef UNITEMP_LM75
#define UNITEMP_LM75

#include "../unitemp.h"
#include "SensorsDriver.h"
#include "../Sensors.h"

/**
 * @brief Выделение памяти и установка начальных значений датчика LM75
 * 
 * @param sensor Указатель на создаваемый датчик
 * @return Истина при успехе
 */
bool unitemp_LM75_alloc(Sensor* sensor);

/**
 * @brief Инициализации датчика LM75
 * 
 * @param sensor Указатель на датчик (тип Sensor)
 * @return Истина если инициализация упспешная
 */
bool unitemp_LM75_init(void* sensor);

/**
 * @brief Деинициализация датчика
 * 
 * @param sensor Указатель на датчик (тип Sensor)
 */
bool unitemp_LM75_deinit(void* s);

/**
 * @brief Обновление значений из датчика
 * 
 * @param sensor Указатель на датчик (тип Sensor)
 * @return Истина если данныее были получены
 */
UnitempStatus unitemp_LM75_update(void* s);

/**
 * @brief Высвободить память датчика
 * 
 * @param sensor Указатель на датчик (тип Sensor)
 */
void unitemp_LM75_free(void* sensor);

#endif