#ifndef UNITEMP_BMP280
#define UNITEMP_BMP280
#include "../unitemp.h"
#include "../Sensors.h"
#include "../interfaces/I2CSensor.h"

typedef struct {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
} BMP280_temp_cal;

// typedef struct {
//     uint16_t dig_P1;
//     int16_t dig_P2;
//     int16_t dig_P3;
//     int16_t dig_P4;
//     int16_t dig_P5;
//     int16_t dig_P6;
//     int16_t dig_P7;
//     int16_t dig_P8;
//     int16_t dig_P9;
// } BMP280_press_cal;

typedef struct {
    BMP280_temp_cal temp_cal;
    // BMP280_press_cal press_cal;
} BMP280_instance;

extern const SensorType BMP280;
/**
 * @brief Выделение памяти и установка начальных значений датчика BMP280
 * @param sensor Указатель на создаваемый датчик
 * @return Истина при успехе
 */
bool unitemp_BMP280_alloc(Sensor* sensor, char* args);

/**
 * @brief Инициализации датчика BMP280
 * @param sensor Указатель на датчик
 * @return Истина если инициализация упспешная
 */
bool unitemp_BMP280_init(Sensor* sensor);

/**
 * @brief Деинициализация датчика
 * @param sensor Указатель на датчик
 */
bool unitemp_BMP280_deinit(Sensor* sensor);

/**
 * @brief Обновление значений из датчика
 * @param sensor Указатель на датчик
 * @return Статус опроса датчика
 */
UnitempStatus unitemp_BMP280_update(Sensor* sensor);

/**
 * @brief Высвободить память датчика
 * @param sensor Указатель на датчик
 */
bool unitemp_BMP280_free(Sensor* sensor);

#endif