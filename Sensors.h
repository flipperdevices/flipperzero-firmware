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
typedef bool(SensorAllocator)(Sensor* sensor, uint8_t* anotherValues);
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
    const char* name;
    SensorAllocator* allocator;
    //Функция высвыбождения памяти для датчика
    SensorFree* mem_releaser;
    //Функция обновления значения датчка
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

/**
 * @brief Конвертация номера порта на корпусе FZ в GPIO 
 * 
 * @param name Номер порта на корпусе FZ
 * @return Указатель на GPIO при успехе, NULL при ошибке
 */
const GPIO* unitemp_GPIO_getFromInt(uint8_t name);

/**
 * @brief Конвертация GPIO в номер на корпусе FZ
 * 
 * @param gpio Указатель на порт
 * @return Номер порта на корпусе FZ
 */
uint8_t unitemp_GPIO_toInt(const GpioPin* gpio);

/**
 * @brief Выделение памяти под датчик
 * 
 * @param name Имя датчика
 * @param st Тип датчика
 * @param anotherValues Массив других различных значений
 * @return Указатель на датчик
 */
Sensor* unitemp_sensor_alloc(char* name, const SensorType* type, uint8_t* anotherValues);

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
UnitempStatus unitemp_sensor_updateData(Sensor* sensor);

/**
 * @brief Функция загрузки датчиков с SD-карты
 * 
 * @return Истина если загрузка прошла успешно
 */
bool unitemp_sensors_load();

/**
 * @brief Сохранение настроек на SD-карту
 * 
 * @return Истина если сохранение прошло успешно
 */
bool unitemp_sensors_save(void);

/**
 * @brief Обновить данные со всех датчиков
 */
void unitemp_sensors_updateValues(void);

/**
 * @brief Высвыбождение памяти после датчиков
 */
void unitemp_sensors_free(void);

const SensorType* unitemp_getTypeFromInt(int type);
#endif