#include "Sensors.h"
#include "OneWire.h"

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

const GPIO* unitemp_getGPIOFormInt(uint8_t name) {
    for(uint8_t i = 0; i < GPIO_ITEMS; i++) {
        if(GPIOList[i].num == name) {
            return &GPIOList[i];
        }
    }
    return NULL;
}

Sensor* unitemp_sensor_alloc(char* name, SensorType st) {
    //Выделение памяти под датчик
    Sensor* sensor = malloc(sizeof(Sensor));
    sensor->name = name;
    //Выделение памяти под инстанс DHT11, DHT12 (1W), DHT21, DHT22, AM2320 (1W)
    if(st == DHT11 || st == DHT12_1W || st == DHT21 || st == DHT22 || st == AM2320_1W) {
        if(sensor == NULL) return false;
        OneWireSensor* instance = malloc(sizeof(OneWireSensor));
        instance->interface = ONE_WIRE;
        instance->lastPollingTime = 0;
        sensor->instance = instance;
        sensor->type = st;
    }
    return sensor;
}

bool unitemp_sensors_init(void) {
    bool result = true;
    //Включение 5V если на порту 1 FZ его нет
    if(furi_hal_power_is_otg_enabled() != true) {
        furi_hal_power_enable_otg();
        FURI_LOG_D(APP_NAME, "OTG enabled");
    }

    //Перебор датчиков из списка
    for(size_t i = 0; i < app->sensors_count; i++) {
        if(app->sensors[i]->interface == ONE_WIRE) {
            if(!unitemp_oneWire_sensorInit(app->sensors[i]->instance)) {
                FURI_LOG_W(
                    APP_NAME,
                    "An error occurred during sensor initialization %s",
                    app->sensors[i]->name);
                result = false;
            }
        }
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
        if(app->sensors[i]->interface == ONE_WIRE) {
            if(!unitemp_oneWire_sensorDeInit(app->sensors[i]->instance)) {
                FURI_LOG_W(
                    APP_NAME,
                    "An error occurred during sensor initialization %s",
                    app->sensors[i]->name);
                result = false;
            }
        }
    }
    return result;
}

UnitempStatus unitemp_sensor_getValues(Sensor* sensor) {
    if(sensor == NULL) return UT_ERROR;

    if(sensor->interface == ONE_WIRE) {
        return unitemp_oneWire_getData(sensor);
    }
    return UT_ERROR;
}