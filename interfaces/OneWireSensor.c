#include "OneWireSensor.h"
#include "../Sensors.h"

//Интервал опроса датчиков (мс)
#define POLLING_INTERVAL 2000
//Максимальное количество попугаев ожидания датчика
#define POLLING_TIMEOUT_TICKS 10000

void unitemp_oneWire_sensorAlloc(Sensor* sensor, SensorType st) {
    OneWireSensor* instance = malloc(sizeof(OneWireSensor));
    instance->interface = ONE_WIRE;
    instance->lastPollingTime = 0xFFFFFFFF;

    sensor->initializer = unitemp_oneWire_sensorInit;
    sensor->deinitializer = unitemp_oneWire_sensorDeInit;
    sensor->updater = unitemp_oneWire_updateData;

    sensor->instance = instance;
    sensor->type = st;
}

bool unitemp_oneWire_sensorInit(void* sensor) {
    OneWireSensor* instance = ((Sensor*)sensor)->instance;
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

bool unitemp_oneWire_sensorDeInit(void* sensor) {
    OneWireSensor* instance = ((Sensor*)sensor)->instance;
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

bool unitemp_oneWire_sensorSetGPIO(Sensor* sensor, const GPIO* gpio) {
    if(sensor == NULL || gpio == NULL) return false;
    OneWireSensor* instance = sensor->instance;
    instance->gpio = gpio;
    return true;
}
const GPIO* unitemp_oneWire_sensorGetGPIO(Sensor* sensor) {
    if(sensor == NULL) return NULL;
    OneWireSensor* instance = sensor->instance;
    return instance->gpio;
}

UnitempStatus unitemp_oneWire_updateData(void* sensor) {
    OneWireSensor* instance = ((Sensor*)sensor)->instance;
    //Проверка на допустимость опроса датчика
    if(furi_get_tick() - instance->lastPollingTime < POLLING_INTERVAL) {
        //Возврат ошибки если последний опрос датчика был неудачным
        if(instance->lastHum == -128.0f && instance->lastTemp == -128.0f) {
            ((Sensor*)sensor)->hum = instance->lastHum;
            ((Sensor*)sensor)->temp = instance->lastTemp;
            return UT_TIMEOUT;
        }

        //Выход в случае раннего опроса
        ((Sensor*)sensor)->hum = instance->lastHum;
        ((Sensor*)sensor)->temp = instance->lastTemp;
        return UT_EARLYPOOL;
    }

    //Массив для приёма данных
    uint8_t data[5] = {0};

    //Сохранение времени последнего опроса
    instance->lastPollingTime = furi_get_tick();

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
            //Запись неправильных значений
            instance->lastHum = -128.0f;
            instance->lastTemp = -128.0f;
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
            //Запись неправильных значений
            instance->lastHum = -128.0f;
            instance->lastTemp = -128.0f;
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
            //Запись неправильных значений
            instance->lastHum = -128.0f;
            instance->lastTemp = -128.0f;
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
            //Запись неправильных значений
            instance->lastHum = -128.0f;
            instance->lastTemp = -128.0f;
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
        //Запись неправильных значений
        instance->lastHum = -128.0f;
        instance->lastTemp = -128.0f;
        //Если контрольная сумма не совпала, возврат ошибки
        return UT_BADCRC;
    }

    /* Преобразование данных в явный вид */
    //DHT11 и DHT12
    if(((Sensor*)sensor)->type == DHT11 || ((Sensor*)sensor)->type == DHT12_1W) {
        instance->lastHum = (float)data[0];
        instance->lastTemp = (float)data[2];

        //Проверка на отрицательность температуры
        if(data[3] != 0) {
            //Проверка знака
            if(!(data[3] & (1 << 7))) {
                //Добавление положительной дробной части
                instance->lastTemp += data[3] * 0.1f;
            } else {
                //А тут делаем отрицательное значение
                data[3] &= ~(1 << 7);
                instance->lastTemp += data[3] * 0.1f;
                instance->lastTemp *= -1;
            }
        }
    }

    //DHT21, DHT22, AM2320
    if(((Sensor*)sensor)->type == DHT21 || ((Sensor*)sensor)->type == DHT22 ||
       ((Sensor*)sensor)->type == AM2320_1W) {
        instance->lastHum = (float)(((uint16_t)data[0] << 8) | data[1]) / 10;
        //Проверка на отрицательность температуры
        if(!(data[2] & (1 << 7))) {
            instance->lastTemp = (float)(((uint16_t)data[2] << 8) | data[3]) / 10;
        } else {
            data[2] &= ~(1 << 7);
            instance->lastTemp = (float)(((uint16_t)data[2] << 8) | data[3]) / 10 * -1;
        }
    }

    ((Sensor*)sensor)->hum = instance->lastHum;
    ((Sensor*)sensor)->temp = instance->lastTemp;
    //Возврат признака успешного опроса
    return UT_OK;
}