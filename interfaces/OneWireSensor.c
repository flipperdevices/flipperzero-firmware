//Использован код Погребняка Дмитрия: https://aterlux.ru/article/1wire

#include "OneWireSensor.h"
#include <furi.h>
#include <furi_hal.h>
#include <one_wire/one_wire_host.h>

const SensorType DS18x2x = {
    .typename = "DS18x2x",
    .interface = &ONE_WIRE,
    .pollingInterval = 250,
    .allocator = unitemp_OneWire_sensor_alloc,
    .mem_releaser = unitemp_OneWire_sensor_free,
    .initializer = unitemp_OneWire_sensor_init,
    .deinitializer = unitemp_OneWire_sensor_deinit,
    .updater = unitemp_OneWire_sensor_update};

OneWireBus* uintemp_OneWire_bus_alloc(const GPIO* gpio) {
    if(gpio == NULL) {
        return NULL;
    }

    //Проверка на наличие шины на этом порте
    for(uint8_t i = 0; i < app->sensors_count; i++) {
        if(app->sensors[i]->type == &DS18x2x &&
           ((OneWireBus*)(app->sensors[i]->instance))->gpio == gpio) {
            ((OneWireBus*)(app->sensors[i]->instance))->device_count++;
            return ((OneWireBus*)(app->sensors[i]->instance));
        }
    }
    unitemp_gpio_lock(gpio, &ONE_WIRE);
    //Высокий уровень по умолчанию
    furi_hal_gpio_write(gpio->pin, true);
    //Режим работы - OpenDrain, подтяжка включается на всякий случай
    furi_hal_gpio_init(
        gpio->pin, //Порт FZ
        GpioModeOutputOpenDrain, //Режим работы - открытый сток
        GpioPullUp, //Принудительная подтяжка линии данных к питанию
        GpioSpeedVeryHigh); //Скорость работы - максимальная

    OneWireBus* bus = malloc(sizeof(OneWireBus));
    bus->device_count = 1;
    bus->gpio = gpio;
    bus->powerMode = PWR_ACTIVE;

    return bus;
}

bool unitemp_oneWire_sensor_start(OneWireSensor* instance) {
    furi_hal_gpio_write(instance->bus->gpio->pin, false);
    furi_delay_us(500);

    furi_hal_gpio_write(instance->bus->gpio->pin, true);

    //Ожидание подъёма шины
    uint32_t t = furi_get_tick();
    while(!furi_hal_gpio_read(instance->bus->gpio->pin)) {
        //Выход если шина не поднялась
        if(furi_get_tick() - t > 100) return false;
    }

    furi_delay_us(100);
    bool status = !furi_hal_gpio_read(instance->bus->gpio->pin);
    furi_delay_us(400);
    return status;
}

static void oneWire_send_bit(OneWireSensor* instance, bool state) {
    if(state) {
        // write 1
        furi_hal_gpio_write(instance->bus->gpio->pin, false);
        furi_delay_us(1);
        furi_hal_gpio_write(instance->bus->gpio->pin, true);
        furi_delay_us(90);
    } else {
        furi_hal_gpio_write(instance->bus->gpio->pin, false);
        furi_delay_us(90);
        furi_hal_gpio_write(instance->bus->gpio->pin, true);
        //Ожидание подъёма шины
        uint32_t t = furi_get_tick();
        while(!furi_hal_gpio_read(instance->bus->gpio->pin)) {
            //Выход если шина не поднялась
            if(furi_get_tick() - t > 100) return;
        }
    }
}

/**
 * @brief Запись байта на шину One Wire
 * 
 * @param instance Указатель на инстанс датчика
 * @param data Записываемый байт
 */
static void oneWire_write(OneWireSensor* instance, uint8_t data) {
    for(int i = 0; i < 8; i++) {
        oneWire_send_bit(instance, (data & (1 << i)) != 0);
    }
}

/**
 * @brief Чтение бита на шине One Wire
 * 
 * @param instance Указатель на инстанс датчика
 * @return Логический уровень бита
 */
static bool oneWire_read_bit(OneWireSensor* instance) {
    furi_hal_gpio_write(instance->bus->gpio->pin, false);
    furi_delay_us(2); // Длительность низкого уровня, минимум 1 мкс
    furi_hal_gpio_write(instance->bus->gpio->pin, true);
    furi_delay_us(8); // Пауза до момента сэмплирования, всего не более 15 мкс
    bool r = furi_hal_gpio_read(instance->bus->gpio->pin);
    furi_delay_us(80); // Ожидание до следующего тайм-слота, минимум 60 мкс с начала низкого уровня
    return r;
}

/**
 * @brief Чтение байта с шины One Wire
 * 
 * @param instance Указатель на инстанс датчика
 * @return Байт информации
 */
static uint8_t oneWire_read(OneWireSensor* instance) {
    uint8_t r = 0;
    for(uint8_t p = 8; p; p--) {
        r >>= 1;
        if(oneWire_read_bit(instance)) r |= 0x80;
    }
    return r;
}
/**
 * @brief Чтение массива байт с шины One Wire
 * 
 * @param instance Указатель на инстанс датчика
 * @param data Указатель на массив, куда будут записаны данные
 * @param len Количество байт
 */
static void oneWire_readBytes(OneWireSensor* instance, uint8_t* data, uint8_t len) {
    for(uint8_t i = 0; i < len; i++) {
        data[i] = oneWire_read(instance);
    }
}

static uint8_t onewire_CRC_update(uint8_t crc, uint8_t b) {
    for(uint8_t p = 8; p; p--) {
        crc = ((crc ^ b) & 1) ? (crc >> 1) ^ 0b10001100 : (crc >> 1);
        b >>= 1;
    }
    return crc;
}

static bool onewire_CRC_check(uint8_t* data, uint8_t len) {
    uint8_t crc = 0;
    for(uint8_t i = 0; i < len; i++) {
        crc = onewire_CRC_update(crc, data[i]);
    }
    return !crc;
}

/**
 * @brief Запись массива байт на шину One Wire
 * 
 * @param instance Указатель на инстанс датчика
 * @param data Указатель на массив, откуда будут записаны данные
 * @param len Количество байт
 */
static void oneWire_writeBytes(OneWireSensor* instance, uint8_t* data, uint8_t len) {
    for(uint8_t i = 0; i < len; i++) {
        oneWire_write(instance, data[i]);
    }
}

/**
 * @brief Чтение индификатора датчика. ID запишется в инстанс датчика
 * 
 * @param instance Указатель на инстанс датчика
 * @return Истина, если код успешно прочитан, ложь если устройство отсутствует или устройств на шине больше одного
 */
static bool oneWire_sensor_readID(OneWireSensor* instance) {
    if(!unitemp_oneWire_sensor_start(instance)) return false;
    oneWire_write(instance, 0x33); // Чтение ПЗУ
    oneWire_readBytes(instance, instance->deviceID, 8);
    if(!onewire_CRC_check(instance->deviceID, 8)) {
        memset(instance->deviceID, 0, 8);
        return false;
    }
    instance->familyCode = instance->deviceID[0];
    return true;
}

// Переменные для хранения промежуточного результата поиска
uint8_t onewire_enum[8]; // найденный восьмибайтовый адрес
uint8_t onewire_enum_fork_bit; // последний нулевой бит, где была неоднозначность (нумеруя с единицы)

// Инициализирует процедуру поиска адресов устройств
void onewire_enum_init(void) {
    for(uint8_t p = 0; p < 8; p++) {
        onewire_enum[p] = 0;
    }
    onewire_enum_fork_bit = 65; // правее правого
}

// Перечисляет устройства на шине 1-wire и получает очередной адрес.
// Возвращает указатель на буфер, содержащий восьмибайтовое значение адреса, либо NULL, если поиск завешён
uint8_t* onewire_enum_next(OneWireSensor* instance) {
    furi_delay_ms(10);
    if(!onewire_enum_fork_bit) { // Если на предыдущем шаге уже не было разногласий
        FURI_LOG_D(APP_NAME, "All devices is found");
        return 0; // то просто выходим ничего не возвращая
    }
    if(!unitemp_oneWire_sensor_start(instance)) {
        FURI_LOG_D(APP_NAME, "Wire is empty");
        return 0;
    }
    uint8_t bp = 8;
    uint8_t* pprev = &onewire_enum[0];
    uint8_t prev = *pprev;
    uint8_t next = 0;

    uint8_t p = 1;
    oneWire_write(instance, 0xF0);
    uint8_t newfork = 0;
    for(;;) {
        uint8_t not0 = oneWire_read_bit(instance);
        uint8_t not1 = oneWire_read_bit(instance);
        if(!not0) { // Если присутствует в адресах бит ноль
            if(!not1) { // Но также присустствует бит 1 (вилка)
                if(p <
                   onewire_enum_fork_bit) { // Если мы левее прошлого правого конфликтного бита,
                    if(prev & 1) {
                        next |= 0x80; // то копируем значение бита из прошлого прохода
                    } else {
                        newfork = p; // если ноль, то запомним конфликтное место
                    }
                } else if(p == onewire_enum_fork_bit) {
                    next |=
                        0x80; // если на этом месте в прошлый раз был правый конфликт с нулём, выведем 1
                } else {
                    newfork = p; // правее - передаём ноль и запоминаем конфликтное место
                }
            } // в противном случае идём, выбирая ноль в адресе
        } else {
            if(!not1) { // Присутствует единица
                next |= 0x80;
            } else { // Нет ни нулей ни единиц - ошибочная ситуация
                FURI_LOG_D(APP_NAME, "Wrong situation");
                return 0;
            }
        }
        oneWire_send_bit(instance, next & 0x80);
        bp--;
        if(!bp) {
            *pprev = next;
            if(p >= 64) break;
            next = 0;
            pprev++;
            prev = *pprev;
            bp = 8;
        } else {
            if(p >= 64) break;
            prev >>= 1;
            next >>= 1;
        }
        p++;
    }
    onewire_enum_fork_bit = newfork;
    return &onewire_enum[0];
}

void unitemp_OneWire_sensor_select(OneWireSensor* instance) {
    oneWire_write(instance, 0x55);
    oneWire_writeBytes(instance, instance->deviceID, 8);
}

bool unitemp_OneWire_sensor_alloc(Sensor* sensor, uint8_t* anotherValues) {
    OneWireSensor* instance = malloc(sizeof(OneWireSensor));
    if(instance == NULL) {
        FURI_LOG_E(APP_NAME, "Sensor %s instance allocation error", sensor->name);
        return false;
    }
    sensor->instance = instance;
    //Очистка адреса
    memset(instance->deviceID, 0, 8);

    instance->bus = uintemp_OneWire_bus_alloc(unitemp_GPIO_getFromInt(anotherValues[0]));
    if(instance != NULL) {
        return true;
    }
    FURI_LOG_E(APP_NAME, "Sensor %s bus allocation error", sensor->name);
    free(instance);
    return false;
}

bool unitemp_OneWire_sensor_free(Sensor* sensor) {
    if(((OneWireSensor*)sensor->instance)->bus->device_count == 0) {
        unitemp_gpio_unlock(((OneWireSensor*)sensor->instance)->bus->gpio);
        free(((OneWireSensor*)sensor->instance)->bus);
    }
    free(sensor->instance);

    return true;
}

bool unitemp_OneWire_sensor_init(Sensor* sensor) {
    OneWireSensor* instance = sensor->instance;
    if(instance == NULL || instance->bus->gpio == NULL) {
        FURI_LOG_E(APP_NAME, "Sensor pointer is null!");
        return false;
    }

    // onewire_enum_init();
    // uint8_t* ids = onewire_enum_next(instance);

    // while(ids != NULL) {
    //     FURI_LOG_D(
    //         APP_NAME,
    //         "Found sensor's ID: %02X%02X%02X%02X%02X%02X%02X%02X",
    //         ids[0],
    //         ids[1],
    //         ids[2],
    //         ids[3],
    //         ids[4],
    //         ids[5],
    //         ids[6],
    //         ids[7]);
    //     ids = onewire_enum_next(instance);
    // }
    //Если ID не инициализирован, то чтение ID из датчика
    furi_delay_ms(1);
    if(instance->familyCode == 0) {
        if(!oneWire_sensor_readID(instance)) {
            return false;
        }
        FURI_LOG_D(
            APP_NAME,
            "%s sensor ID: %02X%02X%02X%02X%02X%02X%02X%02X",
            sensor->name,
            instance->deviceID[0],
            instance->deviceID[1],
            instance->deviceID[2],
            instance->deviceID[3],
            instance->deviceID[4],
            instance->deviceID[5],
            instance->deviceID[6],
            instance->deviceID[7]);
    }

    if(instance->familyCode == FC_DS18B20 || instance->familyCode == FC_DS1822) {
        //Установка разрядности в 10 бит
        if(!unitemp_oneWire_sensor_start(instance)) return false;
        unitemp_OneWire_sensor_select(instance);
        oneWire_write(instance, 0x4E); // Запись в память
        uint8_t buff[3];
        buff[0] = 0x4B; //Значение нижнего предела
        buff[1] = 0x46; //Значение высшего предела
        buff[2] = 0x3F; //10 бит разрядность преобразования
        oneWire_writeBytes(instance, buff, 3);

        //Сохранение значений в EEPROM для автоматического восстановления после сбоев питания
        if(!unitemp_oneWire_sensor_start(instance)) return false;
        unitemp_OneWire_sensor_select(instance);
        oneWire_write(instance, 0x48); // Запись в EEPROM
    }

    return true;
}

bool unitemp_OneWire_sensor_deinit(Sensor* sensor) {
    OneWireSensor* instance = sensor->instance;
    if(instance == NULL || instance->bus->gpio == NULL) return false;
    instance->bus->device_count--;
    if(instance->bus->device_count == 0) {
        //Низкий уровень по умолчанию
        furi_hal_gpio_write(instance->bus->gpio->pin, false);
        //Режим работы - аналог, подтяжка выключена
        furi_hal_gpio_init(
            instance->bus->gpio->pin, //Порт FZ
            GpioModeAnalog, //Режим работы - аналог
            GpioPullNo, //Подтяжка выключена
            GpioSpeedLow); //Скорость работы - минимальная
    }

    return true;
}

UnitempStatus unitemp_OneWire_sensor_update(Sensor* sensor) {
    OneWireSensor* instance = sensor->instance;
    if(sensor->status != UT_POLLING) {
        if(!unitemp_oneWire_sensor_start(instance)) return UT_TIMEOUT;
        unitemp_OneWire_sensor_select(instance);
        oneWire_write(instance, 0x44); // convert t
        if(instance->bus->powerMode == PWR_PASSIVE) {
            furi_hal_gpio_write(instance->bus->gpio->pin, true);
            furi_hal_gpio_init(
                instance->bus->gpio->pin, GpioModeOutputPushPull, GpioPullUp, GpioSpeedVeryHigh);
        }
        return UT_POLLING;
    } else {
        if(instance->bus->powerMode == PWR_PASSIVE) {
            furi_hal_gpio_write(instance->bus->gpio->pin, true);
            furi_hal_gpio_init(
                instance->bus->gpio->pin, GpioModeOutputOpenDrain, GpioPullUp, GpioSpeedVeryHigh);
        }
        if(!unitemp_oneWire_sensor_start(instance)) return UT_TIMEOUT;
        unitemp_OneWire_sensor_select(instance);
        oneWire_write(instance, 0xBE); // Read Scratch-pad
        uint8_t buff[9];
        oneWire_readBytes(instance, buff, 9);
        if(!onewire_CRC_check(buff, 9)) {
            return UT_TIMEOUT;
        }
        int16_t raw = buff[0] | ((int16_t)buff[1] << 8);
        if(instance->familyCode == FC_DS18S20) {
            sensor->temp = (float)raw / 2.0f;
        } else {
            sensor->temp = (float)raw / 16.0f;
        }
    }

    return UT_OK;
}