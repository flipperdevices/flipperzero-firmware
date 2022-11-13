#include "Sensors.h"
#include "./interfaces/SingleWireSensor.h"
#include "./interfaces/I2CSensor.h"
#include "./sensors/SensorsDriver.h"

#include <furi_hal_power.h>

//Порты ввода/вывода, которые не были обозначены в общем списке
const GpioPin SWC_10 = {.pin = LL_GPIO_PIN_14, .port = GPIOA};
const GpioPin SIO_12 = {.pin = LL_GPIO_PIN_13, .port = GPIOA};
const GpioPin TX_13 = {.pin = LL_GPIO_PIN_6, .port = GPIOB};
const GpioPin RX_14 = {.pin = LL_GPIO_PIN_7, .port = GPIOB};

//Количество доступных портов ввода/вывода
#define GPIO_ITEMS (sizeof(GPIOList) / sizeof(GPIO))
//Количество интерфейсов
#define INTERFACES_TYPES_COUNT (int)(sizeof(interfaces) / sizeof(const Interface*))
//Количество типов датчиков
#define SENSOR_TYPES_COUNT (int)(sizeof(sensorTypes) / sizeof(const SensorType*))

//Перечень достуных портов ввода/вывода
static const GPIO GPIOList[] = {
    {2, "2 (A7)", &gpio_ext_pa7},
    {3, "3 (A6)", &gpio_ext_pa6},
    {4, "4 (A4)", &gpio_ext_pa4},
    {5, "5 (B3)", &gpio_ext_pb3},
    {6, "6 (B2)", &gpio_ext_pb2},
    {7, "7 (C3)", &gpio_ext_pc3},
    {10, " 10(SWC) ", &SWC_10},
    {12, "12 (SIO)", &SIO_12},
    {13, "13 (TX)", &TX_13},
    {14, "14 (RX)", &RX_14},
    {15, "15 (C1)", &gpio_ext_pc1},
    {16, "16 (C0)", &gpio_ext_pc0},
    {17, "17 (1W)", &ibutton_gpio}};

const Interface SINGLE_WIRE = {
    .name = "Single wire",
    .allocator = unitemp_singleWire_alloc,
    .mem_releaser = unitemp_singleWire_free,
    .updater = unitemp_singleWire_update};
const Interface I2C = {
    .name = "I2C",
    .allocator = unitemp_I2C_sensor_alloc,
    .mem_releaser = unitemp_I2C_sensor_free,
    .updater = unitemp_I2C_sensor_update};

//Перечень интерфейсов подключения
//static const Interface* interfaces[] = {&SINGLE_WIRE, &I2C};
//Перечень датчиков
static const SensorType* sensorTypes[] =
    {&DHT11, &DHT12_SW, &DHT21, &DHT22, &AM2320_SW, &LM75, &BMP280};

const SensorType* unitemp_getTypeFromInt(int type) {
    if(type > SENSOR_TYPES_COUNT) return NULL;
    return sensorTypes[type];
}

int unitemp_getIntFromType(const SensorType* type) {
    for(int i = 0; i < SENSOR_TYPES_COUNT; i++) {
        if(!strcmp(type->typename, sensorTypes[i]->typename)) {
            return i;
        }
    }
    return 255;
}
const GPIO* unitemp_GPIO_getFromInt(uint8_t name) {
    for(uint8_t i = 0; i < GPIO_ITEMS; i++) {
        if(GPIOList[i].num == name) {
            return &GPIOList[i];
        }
    }
    return NULL;
}

uint8_t unitemp_GPIO_toInt(const GpioPin* gpio) {
    if(gpio == NULL) return 255;
    for(uint8_t i = 0; i < GPIO_ITEMS; i++) {
        if(GPIOList[i].pin->pin == gpio->pin && GPIOList[i].pin->port == gpio->port) {
            return GPIOList[i].num;
        }
    }
    return 255;
}

bool unitemp_sensors_load() {
    FURI_LOG_D(APP_NAME, "Loading sensors...");
    app->sensors_count = 0;
    memset(app->sensors, 0, sizeof(app->sensors));

    //Выделение памяти на поток
    app->file_stream = file_stream_alloc(app->storage);

    //Переменная пути к файлу
    char filepath[sizeof(APP_PATH_FOLDER) + sizeof(APP_FILENAME_SENSORS)] = {0};
    //Составление пути к файлу
    strcpy(filepath, APP_PATH_FOLDER);
    strcat(filepath, "/");
    strcat(filepath, APP_FILENAME_SENSORS);

    //Открытие потока к файлу с датчиками
    if(!file_stream_open(app->file_stream, filepath, FSAM_READ_WRITE, FSOM_OPEN_EXISTING)) {
        if(file_stream_get_error(app->file_stream) == FSE_NOT_EXIST) {
            FURI_LOG_W(APP_NAME, "Missing sensors file");
            //Закрытие потока и освобождение памяти
            file_stream_close(app->file_stream);
            stream_free(app->file_stream);
            return false;
        } else {
            FURI_LOG_E(
                APP_NAME,
                "An error occurred while loading the sensors file: %d",
                file_stream_get_error(app->file_stream));
            //Закрытие потока и освобождение памяти
            file_stream_close(app->file_stream);
            stream_free(app->file_stream);
            return false;
        }
    }

    //Вычисление размера файла
    size_t file_size = stream_size(app->file_stream);
    FURI_LOG_D(APP_NAME, "Sensors file size: %d bytes", file_size);
    //Если файл пустой, то:
    if(file_size == (size_t)0) {
        FURI_LOG_W(APP_NAME, "Sensors file is empty");
        //Закрытие потока и освобождение памяти
        file_stream_close(app->file_stream);
        stream_free(app->file_stream);
        return false;
    }
    //Выделение памяти под загрузку файла
    uint8_t* file_buf = malloc(file_size);
    //Опустошение буфера файла
    memset(file_buf, 0, file_size);
    //Загрузка файла
    if(stream_read(app->file_stream, file_buf, file_size) != file_size) {
        //Выход при ошибке чтения
        FURI_LOG_E(APP_NAME, "Error reading sensors file");
        //Закрытие потока и освобождение памяти
        file_stream_close(app->file_stream);
        stream_free(app->file_stream);
        free(file_buf);
        return false;
    }

    //Построчное чтение файла
    char* line = strtok((char*)file_buf, "\n");
    while(line != NULL) {
        char name[11] = {0};
        int type = 255, otherValue = 255;
        sscanf(line, "%s %d %d", name, &type, &otherValue);
        //Ограничение длины имени
        name[10] = '\0';
        FURI_LOG_D(APP_NAME, "%s %d %d", name, type, otherValue);

        uint16_t otherValues[] = {otherValue};
        //Проверка типа датчика
        if(type < SENSOR_TYPES_COUNT && sizeof(name) <= 11) {
            unitemp_sensor_alloc(name, unitemp_getTypeFromInt(type), otherValues);
        } else {
            FURI_LOG_E(APP_NAME, "Unsupported sensor name (%s) or sensor type (%d)", name, type);
        }
        line = strtok((char*)NULL, "\n");
    }

    // uint16_t otherValues[] = {0x76};
    // unitemp_sensor_alloc("BMP280", BMP280, otherValues);

    free(file_buf);
    file_stream_close(app->file_stream);
    stream_free(app->file_stream);

    FURI_LOG_I(APP_NAME, "Sensors have been successfully loaded");
    return true;
}

bool unitemp_sensors_save(void) {
    FURI_LOG_D(APP_NAME, "Saving sensors...");

    //Выделение памяти для потока
    app->file_stream = file_stream_alloc(app->storage);

    //Переменная пути к файлу
    char filepath[sizeof(APP_PATH_FOLDER) + sizeof(APP_FILENAME_SENSORS)] = {0};
    //Составление пути к файлу
    strcpy(filepath, APP_PATH_FOLDER);
    strcat(filepath, "/");
    strcat(filepath, APP_FILENAME_SENSORS);
    //Создание папки плагина
    storage_common_mkdir(app->storage, APP_PATH_FOLDER);
    //Открытие потока
    if(!file_stream_open(app->file_stream, filepath, FSAM_READ_WRITE, FSOM_CREATE_ALWAYS)) {
        FURI_LOG_E(
            APP_NAME,
            "An error occurred while saving the sensors file: %d",
            file_stream_get_error(app->file_stream));
        //Закрытие потока и освобождение памяти
        file_stream_close(app->file_stream);
        stream_free(app->file_stream);
        return false;
    }

    //Сохранение датчиков
    for(size_t i = 0; i < app->sensors_count; i++) {
        if(app->sensors[i]->type->interface == &SINGLE_WIRE) {
            stream_write_format(
                app->file_stream,
                "%s %d %d\n",
                app->sensors[i]->name,
                unitemp_getIntFromType(app->sensors[i]->type),
                unitemp_GPIO_toInt(unitemp_singleWire_sensorGetGPIO(app->sensors[i])->pin));
        }
        if(app->sensors[i]->type->interface == &I2C) {
            stream_write_format(
                app->file_stream,
                "%s %d %d\n",
                app->sensors[i]->name,
                unitemp_getIntFromType(app->sensors[i]->type),
                ((I2CSensor*)app->sensors[i]->instance)->currentI2CAdr);
        }
    }

    //Закрытие потока и освобождение памяти
    file_stream_close(app->file_stream);
    stream_free(app->file_stream);

    FURI_LOG_I(APP_NAME, "Sensors have been successfully saved");
    return true;
}

Sensor* unitemp_sensor_alloc(char* name, const SensorType* type, uint16_t* anotherValues) {
    bool status = false;
    //Выделение памяти под датчик
    Sensor* sensor = malloc(sizeof(Sensor));
    if(sensor == NULL) {
        FURI_LOG_E(APP_NAME, "Sensor %s allocation error", name);
        return false;
    }

    //Выделение памяти под имя
    sensor->name = malloc(11);
    if(sensor->name == NULL) {
        FURI_LOG_E(APP_NAME, "Sensor %s name allocation error", name);
        return false;
    }
    //Запись имени датчка
    strcpy(sensor->name, name);
    //Тип датчика
    sensor->type = type;
    //Статус датчика по умолчанию - ошибка
    sensor->status = UT_ERROR;
    //Время последнего опроса
    sensor->lastPollingTime =
        furi_get_tick() - 10000; //чтобы первый опрос произошёл как можно раньше

    sensor->temp = -128.0f;
    sensor->hum = -128.0f;

    //Выделение памяти под инстанс датчика в зависимости от его интерфейса
    status = sensor->type->interface->allocator(sensor, anotherValues);

    //Если датчик успешно развёрнут, то добавление его в общий список и выход
    if(status) {
        app->sensors[app->sensors_count++] = sensor;
        return sensor;
    }
    //Если ни один из типов не подошёл, то выход с очисткой
    free(sensor->name);
    free(sensor);
    FURI_LOG_E(APP_NAME, "Sensor %s type is unsupported: %s", name, type->typename);
    return NULL;
}

void unitemp_sensor_free(Sensor* sensor) {
    if(sensor == NULL) {
        FURI_LOG_E(APP_NAME, "Null pointer sensor releasing");
        return;
    }
    if(sensor->type == NULL) {
        FURI_LOG_E(APP_NAME, "Sensor type is null");
        return;
    }
    if(sensor->type->mem_releaser == NULL) {
        FURI_LOG_E(APP_NAME, "Sensor releaser is null");
        return;
    }
    bool status = false;
    //Высвобождение памяти под инстанс
    status = sensor->type->interface->mem_releaser(sensor);
    if(status) {
        FURI_LOG_D(APP_NAME, "Sensor %s memory successfully released", sensor->name);
    } else {
        FURI_LOG_E(APP_NAME, "Sensor %s memory is not released", sensor->name);
    }
    free(sensor->name);
    free(sensor);
}

void unitemp_sensors_free(void) {
    for(size_t i = 0; i < app->sensors_count; i++) {
        unitemp_sensor_free(app->sensors[i]);
    }
    app->sensors_count = 0;
}

bool unitemp_sensors_init(void) {
    bool result = true;

    //Перебор датчиков из списка
    for(size_t i = 0; i < app->sensors_count; i++) {
        //Включение 5V если на порту 1 FZ его нет
        //Может пропасть при отключении USB
        if(furi_hal_power_is_otg_enabled() != true) {
            furi_hal_power_enable_otg();
            FURI_LOG_D(APP_NAME, "OTG enabled");
        }
        if(!(*app->sensors[i]->type->initializer)(app->sensors[i])) {
            FURI_LOG_E(
                APP_NAME,
                "An error occurred during sensor initialization %s",
                app->sensors[i]->name);
            result = false;
        }
        FURI_LOG_D(APP_NAME, "Sensor %s successfully initialized", app->sensors[i]->name);
    }
    return result;
}

bool unitemp_sensors_deInit(void) {
    bool result = true;
    //Выключение 5 В если до этого оно не было включено
    if(app->settings.lastOTGState != true) {
        furi_hal_power_disable_otg();
        FURI_LOG_D(APP_NAME, "OTG disabled");
    }

    //Перебор датчиков из списка
    for(size_t i = 0; i < app->sensors_count; i++) {
        if(!(*app->sensors[i]->type->deinitializer)(app->sensors[i])) {
            FURI_LOG_E(
                APP_NAME,
                "An error occurred during sensor deinitialization %s",
                app->sensors[i]->name);
            result = false;
        }
    }
    return result;
}

UnitempStatus unitemp_sensor_updateData(Sensor* sensor) {
    if(sensor == NULL) return UT_ERROR;

    //Проверка на допустимость опроса датчика
    if(furi_get_tick() - sensor->lastPollingTime < sensor->type->pollingInterval) {
        //Возврат ошибки если последний опрос датчика был неудачным
        if(sensor->status == UT_TIMEOUT) {
            return UT_TIMEOUT;
        }
        return UT_EARLYPOOL;
    }

    sensor->lastPollingTime = furi_get_tick();

    if(!furi_hal_power_is_otg_enabled()) {
        furi_hal_power_enable_otg();
    }

    sensor->status = sensor->type->interface->updater(sensor);

    FURI_LOG_D(APP_NAME, "Sensor %s update status %d", sensor->name, sensor->status);
    if(app->settings.unit == FAHRENHEIT && sensor->status == UT_OK)
        uintemp_celsiumToFarengate(sensor);
    return sensor->status;
}

void unitemp_sensors_updateValues(void) {
    for(size_t i = 0; i < app->sensors_count; i++) {
        unitemp_sensor_updateData(app->sensors[i]);
    }
}
