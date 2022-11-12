// #ifndef UNITEMP_BMP280
// #define UNITEMP_BMP280
// #include "../unitemp.h"
// #include "../Sensors.h"
// #include "../interfaces/I2CSensor.h"
// #include "SensorsDriver.h"

// typedef struct {
//     uint16_t dig_T1;
//     int16_t dig_T2;
//     int16_t dig_T3;
// } BMP280_temp_cal;

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

// typedef struct {
//     BMP280_temp_cal temp_cal;
//     BMP280_press_cal press_cal;

// } BMP280_instance;

// /**
//  * @brief Инициализация датчика
//  *
//  * @param i2c_sensor Указатель на датчик
//  * @return Инстина если инициализация успешная
//  */

// bool BMP280_init(I2CSensor* i2c_sensor);

// bool BMP280_updateData(Sensor* sensor);

// #endif