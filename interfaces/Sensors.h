#ifndef UNITEMP_SENSORS
#define UNITEMP_SENSORS
#include <furi.h>
#include <input/input.h>

//Статусы опроса датчика
typedef enum {
    UT_OK, //Всё хорошо, опрос успешен
    UT_TIMEOUT, //Датчик не отозвался
    UT_EARLYPOOL, //Опрос раньше положенной задержки
    UT_BADCRC, //Неверная контрольная сумма
    UT_ERROR, //Прочие ошибки
} UnitempStatus;

//Типы датчиков
typedef enum {
    DHT11,
    DHT12_1W,
    DHT12_I2C,
    DHT20, //AM2108
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
typedef enum Interface {
    ONE_WIRE, //Собственный однопроводной протокол датчиков DHTXX и AM23XX
    DALLAS, //Однопроводной протокол Dallas
    I2C,
    SPI,

    CONNECTION_TYPES_COUNT //Общее количество типов подключений
} Interface;

//Порт ввода/вывода Flipper Zero
typedef struct GPIO {
    const uint8_t num;
    const char* name;
    const GpioPin* pin;
} GPIO;

//Датчик
typedef struct Sensor {
    //Имя датчика
    char* name;
    //Температура
    float temp;
    //Относительная влажность
    float hum;

    //Тип датчика
    SensorType type;
    //Интерфейсы подключения
    Interface interface;
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
const GPIO* unitemp_GPIO_getFromInt(uint8_t name);

/**
 * @brief Выделение памяти под датчик
 * 
 * @param name Имя датчика
 * @param st Тип датчика
 * @return Указатель на датчик
 */
Sensor* unitemp_sensor_alloc(char* name, SensorType st);

/**
 * @brief Инициализация загруженных датчиков
 * 
 * @return Истина если всё прошло успешно
 */
bool unitemp_sensors_init(void);

/**
 * @brief Деинициализация загруженных датчиков
 * 
 * @return Истина если всё прошло успешно
 */
bool unitemp_sensors_deInit(void);

/**
 * @brief Получение данных указанного датчика
 * 
 * @param sensor Указатель на датчик
 * @return Статус опроса датчика
 */
UnitempStatus unitemp_sensor_getValues(Sensor* sensor);

/**
 * @brief Функция загрузки датчиков с SD-карты
 * 
 * @return Истина если загрузка прошла успешно
 */
bool unitemp_sensors_load();

#endif