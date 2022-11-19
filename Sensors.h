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
    UT_POLLING, //В датчике происходит преобразование
} UnitempStatus;

//Порт ввода/вывода Flipper Zero
typedef struct GPIO {
    const uint8_t num;
    const char* name;
    const GpioPin* pin;
} GPIO;

typedef struct Sensor Sensor;

/**
 * @brief Указатель функции выделения памяти и подготовки экземпляра датчика
 */
typedef bool(SensorAllocator)(Sensor* sensor, char* args);
/**
 * @brief Указатель на функцию высвобождении памяти датчика
 */
typedef bool(SensorFree)(Sensor* sensor);
/**
 * @brief Указатель функции инициализации датчика
 */
typedef bool(SensorInitializer)(Sensor* sensor);
/**
 * @brief Указатель функции деинициализации датчика
 */
typedef bool(SensorDeinitializer)(Sensor* sensor);
/**
 * @brief Указатель функции обновления значения датчика
 */
typedef UnitempStatus(SensorUpdater)(Sensor* sensor);

//Типы подключения датчиков
typedef struct Interface {
    //Имя интерфейса
    const char* name;
    //Функция выделения памяти интерфейса
    SensorAllocator* allocator;
    //Функция высвыбождения памяти интерфейса
    SensorFree* mem_releaser;
    //Функция обновления значения датчика по интерфейсу
    SensorUpdater* updater;
} Interface;

//Типы датчиков
typedef struct {
    //Имя типа датчика
    const char* typename;
    //Интерфейс подключения
    const Interface* interface;
    //Интервал опроса датчика
    uint16_t pollingInterval;
    //Функция выделения памяти для датчика
    SensorAllocator* allocator;
    //Функция высвыбождения памяти для датчика
    SensorFree* mem_releaser;
    //Функция инициализации датчика
    SensorInitializer* initializer;
    //Функция деинициализация датчика
    SensorDeinitializer* deinitializer;
    //Функция обновления значения датчка
    SensorUpdater* updater;
} SensorType;

//Датчик
typedef struct Sensor {
    //Имя датчика
    char* name;
    //Температура
    float temp;
    //Относительная влажность
    float hum;

    //Тип датчика
    const SensorType* type;
    //Статус последнего опроса датчика
    UnitempStatus status;
    //Время последнего опроса датчика
    uint32_t lastPollingTime;
    //Экземпляр датчика
    void* instance;
} Sensor;

extern const Interface SINGLE_WIRE; //Собственный однопроводной протокол датчиков DHTXX и AM23XX
extern const Interface ONE_WIRE; //Однопроводной протокол Dallas
extern const Interface I2C; //I2C_2 (PC0, PC1)
//extern const Interface SPI;

/* ============================= Датчик(и) ============================= */
/**
 * @brief Выделение памяти под датчик
 * 
 * @param name Имя датчика
 * @param type Тип датчика
 * @param args Указатель на строку с парамерами датчика
 * @return Указатель на датчик в случае успешного выделения памяти, NULL при ошибке
 */
Sensor* unitemp_sensor_alloc(char* name, const SensorType* type, char* args);

/**
 * @brief Высвыбождение памяти конкретного датчка
 * @param sensor Указатель на датчик
 */
void unitemp_sensor_free(Sensor* sensor);

/**
 * @brief Обновление данных указанного датчика
 * @param sensor Указатель на датчик
 * @return Статус опроса датчика
 */
UnitempStatus unitemp_sensor_updateData(Sensor* sensor);

/**
 * @brief Загрузка датчиков с SD-карты
 * @return Истина если загрузка прошла успешно
 */
bool unitemp_sensors_load();

/**
 * @brief Функция перезагрузки датчиков с SD-карты
*/
void unitemp_sensors_reload(void);

/**
 * @brief Сохранение настроек на SD-карту
 * @return Истина если сохранение прошло успешно
 */
bool unitemp_sensors_save(void);

/**
 * @brief Инициализация загруженных датчиков
 * @return Истина если всё прошло успешно
 */
bool unitemp_sensors_init(void);

/**
 * @brief Деинициализация загруженных датчиков
 * @return Истина если всё прошло успешно
 */
bool unitemp_sensors_deInit(void);

/**
 * @brief Высвыбождение памяти всех датчиков
 */
void unitemp_sensors_free(void);

/**
 * @brief Обновить данные всех датчиков
 */
void unitemp_sensors_updateValues(void);

/**
* @brief Получить списк доступных типов датчиков
* @return Указатель на список датчиков
*/
const SensorType** unitemp_sensors_getTypes(void);

/**
* @brief Получить количество доступных типов датчиков
* @return Количество доступных типов датчиков
*/
uint8_t unitemp_sensors_getTypesCount(void);

/**
 * @brief Получить тип сенсора по его индексу
 * @param index Индекс типа датчика (от 0 до SENSOR_TYPES_COUNT)
 * @return const SensorType* 
 */
const SensorType* unitemp_sensors_getTypeFromInt(uint8_t index);

/**
 * @brief Преобразовать строчное название датчка в указатель
 * 
 * @param str Имя датчика в виде строки
 * @return Указатель на тип датчика при успехе, иначе NULL
 */
const SensorType* unitemp_sensors_getTypeFromStr(char* str);

/* ============================= GPIO ============================= */
/**
 * @brief Конвертация номера порта на корпусе FZ в GPIO 
 * @param name Номер порта на корпусе FZ
 * @return Указатель на GPIO при успехе, NULL при ошибке
 */
const GPIO* unitemp_gpio_getFromInt(uint8_t name);
/**
 * @brief Конвертация GPIO в номер на корпусе FZ
 * @param gpio Указатель на порт
 * @return Номер порта на корпусе FZ
 */
uint8_t unitemp_gpio_toInt(const GPIO* gpio);

/**
 * @brief Блокировка GPIO указанным интерфейсом
 * @param gpio Указатель на порт
 * @param interface Указатель на интерфейс, которым порт будет занят
 */
void unitemp_gpio_lock(const GPIO* gpio, const Interface* interface);

/**
 * @brief Разблокировка порта
 * @param gpio Указатель на порт
 */
void unitemp_gpio_unlock(const GPIO* gpio);
/**
 * @brief Получить количество доступных портов для указанного интерфейса
 * @param interface Указатель на интерфейс
 * @return Количество доступных портов
 */
uint8_t unitemp_gpio_getAviablePortsCount(const Interface* interface);
/**
 * @brief Получить указатель на доступный для интерфейса порт по индексу 
 * @param interface Указатель на интерфейс
 * @param index Номер порта (от 0 до unitemp_gpio_getAviablePortsCount())
 * @return Указатель на доступный порт
 */
const GPIO* unitemp_gpio_getAviablePort(const Interface* interface, uint8_t index);

/* Датчики */
//DHTxx и их производные
#include "./interfaces/SingleWireSensor.h"
//DS18x2x
#include "./interfaces/OneWireSensor.h"
#include "./sensors/LM75.h"
#include "./sensors/BMP280.h"
#endif
