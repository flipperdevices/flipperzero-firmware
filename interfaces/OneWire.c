#include "OneWire.h"
#include "Sensors.h"

//Интервал опроса датчиков (мс)
#define POLLING_INTERVAL 2000
//Максимальное количество попугаев ожидания датчика
#define POLLING_TIMEOUT_TICKS 10000

bool unitemp_oneWire_sensorInit(OneWireSensor* sensor) {
    if(sensor == NULL || sensor->gpio == NULL) return false;
    //Высокий уровень по умолчанию
    furi_hal_gpio_write(sensor->gpio->pin, true);
    //Режим работы - OpenDrain, подтяжка включается на всякий случай
    furi_hal_gpio_init(
        sensor->gpio->pin, //Порт FZ
        GpioModeOutputOpenDrain, //Режим работы - открытый сток
        GpioPullUp, //Принудительная подтяжка линии данных к питанию
        GpioSpeedVeryHigh); //Скорость работы - максимальная
    return true;
}

bool unitemp_oneWire_sensorDeInit(OneWireSensor* sensor) {
    if(sensor == NULL || sensor->gpio == NULL) return false;
    //Низкий уровень по умолчанию
    furi_hal_gpio_write(sensor->gpio->pin, false);
    //Режим работы - аналог, подтяжка выключена
    furi_hal_gpio_init(
        sensor->gpio->pin, //Порт FZ
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

UnitempStatus unitemp_oneWire_getData(Sensor* sensor) {
    OneWireSensor* instance = sensor->instance;
    //Проверка на допустимость опроса датчика
    if(furi_get_tick() - instance->lastPollingTime < POLLING_INTERVAL) {
        //Выход в случае раннего опроса
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
            sensor->hum = -128.0f;
            sensor->temp = -128.0f;
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
            sensor->hum = -128.0f;
            sensor->temp = -128.0f;
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
            sensor->hum = -128.0f;
            sensor->temp = -128.0f;
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
            sensor->hum = -128.0f;
            sensor->temp = -128.0f;
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
        sensor->hum = -128.0f;
        sensor->temp = -128.0f;
        //Если контрольная сумма не совпала, возврат ошибки
        return UT_BADCRC;
    }

    /* Преобразование данных в явный вид */
    //DHT11 и DHT12
    if(sensor->type == DHT11 || sensor->type == DHT12_1W) {
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
    if(sensor->type == DHT21 || sensor->type == DHT22 || sensor->type == AM2320_1W) {
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