#ifndef UNITEMP_SENSORS
#define UNITEMP_SENSORS
#include <furi.h>
#include <input/input.h>

//Типы датчиков
typedef enum {
    DHT11,
    DHT12_1W,
    DHT12_I2C,
    DHT20,
    DHT21, //AM2301
    DHT22, //AM2302
    AM2320_1W,
    AM2320_I2C,
    LM75,
    DS18B20,
    BMP180,
    BMP280,
    BME280,

    //Порядок сенсоров сверху не менять!
    SENSOR_TYPES_COUNT //Общее количество типов датчиков
} SensorType;

//Типы подключения датчиков
typedef enum {
    ONE_WIRE, //Собственный однопроводной протокол датчиков DHTXX и AM23XX
    DALLAS, //Однопроводной протокол Dallas
    I2C,
    SPI,

    CONNECTION_TYPES_COUNT //Общее количество типов подключений
} Interface;

//Порт ввода/вывода Flipper Zero
typedef struct {
    const uint8_t num;
    const char* name;
    const GpioPin* pin;
} GPIO;

//Датчик
typedef struct {
    //Имя датчика
    char* name;
    //Температура
    float temp;
    //Относительная влажность
    float hum;

    //Тип датчика
    SensorType type;
    //Экземпляр датчика
    void* instance;
} Sensor;

/**
 * @brief Получить имя типа датчика
 * 
 * @param st Тип датчика
 * @return Указатель на строку с именем типа датчика
 */
const char* unitemp_getSensorTypeName(SensorType st);

/**
 * @brief Конвертация номера порта на корпусе FZ в GPIO 
 * 
 * @param name Номер порта на корпусе FZ
 * @return Указатель на GPIO при успехе, NULL при ошибке
 */
const GPIO* unitemp_getGPIOFormInt(uint8_t name);

#endif