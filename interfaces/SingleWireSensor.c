#include "SingleWireSensor.h"
#include "../Sensors.h"

//Интервал опроса датчиков (мс)
#define POLLING_INTERVAL 2000
//Максимальное количество попугаев ожидания датчика
#define POLLING_TIMEOUT_TICKS 10000

/* Типы датчиков и их параметры */
//DHT11
const SensorType DHT11 = {
    .typename = "DHT11",
    .interface = &SINGLE_WIRE,
    .pollingInterval = 2000,
    .allocator = unitemp_singleWire_alloc,
    .mem_releaser = unitemp_singleWire_free,
    .initializer = unitemp_singleWire_init,
    .deinitializer = unitemp_singleWire_deinit,
    .updater = unitemp_singleWire_update};
const SensorType DHT12_SW = {
    .typename = "DHT12 (1 Wire)",
    .interface = &SINGLE_WIRE,
    .pollingInterval = 2000,
    .allocator = unitemp_singleWire_alloc,
    .mem_releaser = unitemp_singleWire_free,
    .initializer = unitemp_singleWire_init,
    .deinitializer = unitemp_singleWire_deinit,
    .updater = unitemp_singleWire_update};

const SensorType DHT21 = {
    .typename = "DHT21",
    .interface = &SINGLE_WIRE,
    .pollingInterval = 2000,
    .allocator = unitemp_singleWire_alloc,
    .mem_releaser = unitemp_singleWire_free,
    .initializer = unitemp_singleWire_init,
    .deinitializer = unitemp_singleWire_deinit,
    .updater = unitemp_singleWire_update};
const SensorType DHT22 = {
    .typename = "DHT22",
    .interface = &SINGLE_WIRE,
    .pollingInterval = 2000,
    .allocator = unitemp_singleWire_alloc,
    .mem_releaser = unitemp_singleWire_free,
    .initializer = unitemp_singleWire_init,
    .deinitializer = unitemp_singleWire_deinit,
    .updater = unitemp_singleWire_update};
const SensorType AM2320_SW = {
    .typename = "AM2320 (1 Wire)",
    .interface = &SINGLE_WIRE,
    .pollingInterval = 2000,
    .allocator = unitemp_singleWire_alloc,
    .mem_releaser = unitemp_singleWire_free,
    .initializer = unitemp_singleWire_init,
    .deinitializer = unitemp_singleWire_deinit,
    .updater = unitemp_singleWire_update};

bool unitemp_singleWire_alloc(void* s, uint16_t* anotherValues) {
    Sensor* sensor = (Sensor*)s;
    SingleWireSensor* instance = malloc(sizeof(SingleWireSensor));
    if(instance == NULL) {
        FURI_LOG_E(APP_NAME, "Sensor %s instance allocation error", sensor->name);
        return false;
    }
    sensor->instance = instance;

    if(unitemp_singleWire_sensorSetGPIO(sensor, unitemp_GPIO_getFromInt(anotherValues[0]))) {
        return true;
    }
    FURI_LOG_E(APP_NAME, "Sensor %s GPIO setting error", sensor->name);
    free(instance);
    return false;
}
bool unitemp_singleWire_free(void* s) {
    Sensor* sensor = (Sensor*)s;
    free(sensor->instance);

    return true;
}

bool unitemp_singleWire_init(void* sensor) {
    SingleWireSensor* instance = ((Sensor*)sensor)->instance;
    if(instance == NULL || instance->gpio == NULL) {
        FURI_LOG_E(APP_NAME, "Sensor pointer is null!");
        return false;
    }
    //Высокий уровень по умолчанию
    furi_hal_gpio_write(instance->gpio->pin, true);
    //Режим работы - OpenDrain, подтяжка включается на всякий случай
    furi_hal_gpio_init(
        instance->gpio->pin, //Порт FZ
        GpioModeOutputOpenDrain, //Режим работы - открытый сток
        GpioPullUp, //Принудительная подтяжка линии данных к питанию
        GpioSpeedVeryHigh); //Скорость работы - максимальная
    return true;
}

bool unitemp_singleWire_deinit(void* sensor) {
    SingleWireSensor* instance = ((Sensor*)sensor)->instance;
    if(instance == NULL || instance->gpio == NULL) return false;
    //Низкий уровень по умолчанию
    furi_hal_gpio_write(instance->gpio->pin, false);
    //Режим работы - аналог, подтяжка выключена
    furi_hal_gpio_init(
        instance->gpio->pin, //Порт FZ
        GpioModeAnalog, //Режим работы - аналог
        GpioPullNo, //Подтяжка выключена
        GpioSpeedLow); //Скорость работы - минимальная
    return true;
}

bool unitemp_singleWire_sensorSetGPIO(Sensor* sensor, const GPIO* gpio) {
    if(sensor == NULL || gpio == NULL) return false;
    SingleWireSensor* instance = sensor->instance;
    instance->gpio = gpio;
    return true;
}
const GPIO* unitemp_singleWire_sensorGetGPIO(Sensor* sensor) {
    if(sensor == NULL) return NULL;
    SingleWireSensor* instance = sensor->instance;
    return instance->gpio;
}

UnitempStatus unitemp_singleWire_update(void* s) {
    Sensor* sensor = (Sensor*)s;
    SingleWireSensor* instance = sensor->instance;

    //Массив для приёма данных
    uint8_t data[5] = {0};

    /* Запрос */
    //Опускание линии
    furi_hal_gpio_write(instance->gpio->pin, false);
    //Ожидание более 18 мс
    furi_delay_ms(19);
    //Выключение прерываний, чтобы ничто не мешало обработке данных
    __disable_irq();
    //Подъём линии
    furi_hal_gpio_write(instance->gpio->pin, true);

    /* Ответ датчика */
    //Переменная-счётчик
    uint16_t timeout = 0;

    //Ожидание подъёма линии
    while(!furi_hal_gpio_read(instance->gpio->pin)) {
        timeout++;
        if(timeout > POLLING_TIMEOUT_TICKS) {
            //Включение прерываний
            __enable_irq();
            //Возврат признака отсутствующего датчика
            return UT_TIMEOUT;
        }
    }
    timeout = 0;

    //Ожидание спада линии
    while(furi_hal_gpio_read(instance->gpio->pin)) {
        timeout++;
        if(timeout > POLLING_TIMEOUT_TICKS) {
            //Включение прерываний
            __enable_irq();
            //Возврат признака отсутствующего датчика
            return UT_TIMEOUT;
        }
    }

    //Ожидание подъёма линии
    while(!furi_hal_gpio_read(instance->gpio->pin)) {
        timeout++;
        if(timeout > POLLING_TIMEOUT_TICKS) {
            //Включение прерываний
            __enable_irq();
            //Возврат признака отсутствующего датчика
            return UT_TIMEOUT;
        }
    }
    timeout = 0;

    //Ожидание спада линии
    while(furi_hal_gpio_read(instance->gpio->pin)) {
        timeout++;
        if(timeout > POLLING_TIMEOUT_TICKS) {
            //Включение прерываний
            __enable_irq();
            //Возврат признака отсутствующего датчика
            return UT_TIMEOUT;
        }
    }

    /* Чтение данных с датчика*/
    //Приём 5 байт
    for(uint8_t a = 0; a < 5; a++) {
        for(uint8_t b = 7; b != 255; b--) {
            uint16_t hT = 0, lT = 0;
            //Пока линия в низком уровне, инкремент переменной lT
            while(!furi_hal_gpio_read(instance->gpio->pin) && lT != 65535) lT++;
            //Пока линия в высоком уровне, инкремент переменной hT
            while(furi_hal_gpio_read(instance->gpio->pin) && hT != 65535) hT++;
            //Если hT больше lT, то пришла единица
            if(hT > lT) data[a] |= (1 << b);
        }
    }
    //Включение прерываний
    __enable_irq();

    //Проверка контрольной суммы
    if((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4]) {
        //Если контрольная сумма не совпала, возврат ошибки
        return UT_BADCRC;
    }

    /* Преобразование данных в явный вид */
    //DHT11 и DHT12
    if(sensor->type == &DHT11 || sensor->type == &DHT12_SW) {
        sensor->hum = (float)data[0];
        sensor->temp = (float)data[2];

        //Проверка на отрицательность температуры
        if(data[3] != 0) {
            //Проверка знака
            if(!(data[3] & (1 << 7))) {
                //Добавление положительной дробной части
                sensor->temp += data[3] * 0.1f;
            } else {
                //А тут делаем отрицательное значение
                data[3] &= ~(1 << 7);
                sensor->temp += data[3] * 0.1f;
                sensor->temp *= -1;
            }
        }
    }

    //DHT21, DHT22, AM2320
    if(sensor->type == &DHT21 || sensor->type == &DHT22 || sensor->type == &AM2320_SW) {
        sensor->hum = (float)(((uint16_t)data[0] << 8) | data[1]) / 10;
        //Проверка на отрицательность температуры
        if(!(data[2] & (1 << 7))) {
            sensor->temp = (float)(((uint16_t)data[2] << 8) | data[3]) / 10;
        } else {
            data[2] &= ~(1 << 7);
            sensor->temp = (float)(((uint16_t)data[2] << 8) | data[3]) / 10 * -1;
        }
    }
    //Возврат признака успешного опроса
    return UT_OK;
}
