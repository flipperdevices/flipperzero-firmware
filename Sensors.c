#include "Sensors.h"
#include "./interfaces/OneWireSensor.h"
#include "./interfaces/I2CSensor.h"

#include <furi_hal_power.h>

//Порты ввода/вывода, которые не были обозначены в общем списке
const GpioPin SWC_10 = {.pin = LL_GPIO_PIN_14, .port = GPIOA};
const GpioPin SIO_12 = {.pin = LL_GPIO_PIN_13, .port = GPIOA};
const GpioPin TX_13 = {.pin = LL_GPIO_PIN_6, .port = GPIOB};
const GpioPin RX_14 = {.pin = LL_GPIO_PIN_7, .port = GPIOB};

//Количество доступных портов ввода/вывода
#define GPIO_ITEMS (sizeof(GPIOList) / sizeof(GPIO))

//Перечень достуных портов ввода/вывода
//static
const GPIO GPIOList[] = {
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

//Перечень имён датчиков
static const char* sensorNames[SENSOR_TYPES_COUNT] = {
    "DHT11",
    "DHT12 (1 Wire)",
    "DHT12 (I2C)",
    "DHT20",
    "DHT21", //AM2301
    "DHT22", //AM2302
    "AM2320 (1W)",
    "AM2320 (I2C)",
    "LM75",
    "DS18B20",
    "BMP180",
    "BMP280",
    "BME280",
};

const char* unitemp_getSensorTypeName(SensorType st) {
    if(st >= SENSOR_TYPES_COUNT) return NULL;
    return sensorNames[st];
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
            unitemp_sensor_alloc(name, type, otherValues);
        }
        line = strtok((char*)NULL, "\n");
    }

    // uint16_t otherValues[] = {0x76};
    // unitemp_sensor_alloc("BMP280", BMP280, otherValues);
    //Адрес 7 бит
    uint16_t otherValues[] = {0b1001000};
    unitemp_sensor_alloc("LM75", LM75, otherValues);

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
        if(app->sensors[i]->interface == ONE_WIRE) {
            stream_write_format(
                app->file_stream,
                "%s %d %d\n",
                app->sensors[i]->name,
                app->sensors[i]->type,
                unitemp_GPIO_toInt(unitemp_oneWire_sensorGetGPIO(app->sensors[i])->pin));
        }
    }

    //Закрытие потока и освобождение памяти
    file_stream_close(app->file_stream);
    stream_free(app->file_stream);

    FURI_LOG_I(APP_NAME, "Sensors have been successfully saved");
    return true;
}

Sensor* unitemp_sensor_alloc(char* name, SensorType st, uint16_t* anotherValues) {
    bool status = false;
    //Выделение памяти под датчик
    Sensor* sensor = malloc(sizeof(Sensor));
    if(sensor == NULL) return false;
    sensor->name = malloc(11);
    strcpy(sensor->name, name);
    sensor->status = UT_ERROR;

    //Выделение памяти под инстанс датчиков One Wire
    if(st == DHT11 || st == DHT12_1W || st == DHT21 || st == DHT22 || st == AM2320_1W) {
        status = unitemp_oneWire_sensorAlloc(sensor, st, anotherValues);
    }
    //Выделение памяти под инстанс датчиков I2C
    if(st == BMP280 || st == LM75) {
        status = unitemp_I2C_sensorAlloc(sensor, st, anotherValues);
    }

    //Если датчик успешно развёрнут, то добавление его в общий список и выход
    if(status) {
        app->sensors[app->sensors_count] = sensor;
        app->sensors_count++;
        return sensor;
    }
    //Если ни один из типов не подошёл, то выход с очисткой
    free(sensor->name);
    free(sensor);
    FURI_LOG_E(APP_NAME, "Sensor %s allocation error", name);
    return NULL;
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
        if(!(*app->sensors[i]->initializer)(app->sensors[i])) {
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
        if(!(*app->sensors[i]->deinitializer)(app->sensors[i])) {
            FURI_LOG_E(
                APP_NAME,
                "An error occurred during sensor deinitialization %s",
                app->sensors[i]->name);
            result = false;
        }
        free(app->sensors[i]);
    }
    return result;
}

UnitempStatus unitemp_sensor_updateData(Sensor* sensor) {
    if(sensor == NULL) return UT_ERROR;

    //Проверка на допустимость опроса датчика
    if(furi_get_tick() - sensor->lastPollingTime < sensor->pollingInterval) {
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

    sensor->status = sensor->updater(sensor);
    if(app->settings.unit == FAHRENHEIT && sensor->status == UT_OK)
        uintemp_celsiumToFarengate(sensor);
    return sensor->status;
}

void unitemp_sensors_updateValues(void) {
    for(size_t i = 0; i < app->sensors_count; i++) {
        unitemp_sensor_updateData(app->sensors[i]);
    }
}
