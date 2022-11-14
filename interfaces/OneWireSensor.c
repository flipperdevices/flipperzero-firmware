#include "OneWireSensor.h"
#include <furi.h>
#include <furi_hal.h>
#include <one_wire/one_wire_host.h>

const SensorType DS18B20 = {
    .typename = "DS18B20",
    .interface = &ONE_WIRE,
    .pollingInterval = 250,
    .allocator = unitemp_OneWire_alloc,
    .mem_releaser = unitemp_OneWire_free,
    .initializer = unitemp_OneWire_init,
    .deinitializer = unitemp_OneWire_deinit,
    .updater = unitemp_OneWire_update};

/**
 * @brief Запуск общения с датчиком 
 * 
 * @param sensor Указатель на датчик 
 * @return Истина если датчик отозвался
 */
static bool oneWire_start(OneWireSensor* instance) {
    furi_hal_gpio_write(instance->gpio->pin, false);
    furi_delay_us(500);

    furi_hal_gpio_write(instance->gpio->pin, true);

    //Ожидание подъёма шины
    uint32_t t = furi_get_tick();
    while(!furi_hal_gpio_read(instance->gpio->pin)) {
        //Выход если шина не поднялась
        if(furi_get_tick() - t > 100) return false;
    }

    furi_delay_us(100);
    bool status = !furi_hal_gpio_read(instance->gpio->pin);
    furi_delay_us(400);
    return status;
}

/**
 * @brief Запись байта на шину One Wire
 * 
 * @param instance Указатель на инстанс датчика
 * @param data Записываемый байт
 */
static void oneWire_write(OneWireSensor* instance, uint8_t data) {
    for(int i = 0; i < 8; i++) {
        if((data & (1 << i)) != 0) {
            // write 1
            furi_hal_gpio_write(instance->gpio->pin, false);
            furi_delay_us(1);
            furi_hal_gpio_write(instance->gpio->pin, true);
            furi_delay_us(90);
        } else {
            furi_hal_gpio_write(instance->gpio->pin, false);
            furi_delay_us(90);
            furi_hal_gpio_write(instance->gpio->pin, true);
            //Ожидание подъёма шины
            uint32_t t = furi_get_tick();
            while(!furi_hal_gpio_read(instance->gpio->pin)) {
                //Выход если шина не поднялась
                if(furi_get_tick() - t > 100) return;
            }
        }
    }
}

/**
 * @brief Чтение бита на шине One Wire
 * 
 * @param instance Указатель на инстанс датчика
 * @return Логический уровень бита
 */
static bool oneWire_read_bit(OneWireSensor* instance) {
    furi_hal_gpio_write(instance->gpio->pin, false);
    furi_delay_us(2); // Длительность низкого уровня, минимум 1 мкс
    furi_hal_gpio_write(instance->gpio->pin, true);
    furi_delay_us(8); // Пауза до момента сэмплирования, всего не более 15 мкс
    bool r = furi_hal_gpio_read(instance->gpio->pin);
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
static void oneWire_readBytes(OneWireSensor* instance, uint8_t* data, size_t len) {
    for(size_t i = 0; i < len; i++) {
        data[i] = oneWire_read(instance);
    }
}

/**
 * @brief Запись массива байт на шину One Wire
 * 
 * @param instance Указатель на инстанс датчика
 * @param data Указатель на массив, откуда будут записаны данные
 * @param len Количество байт
 */
static void oneWire_writeBytes(OneWireSensor* instance, uint8_t* data, size_t len) {
    for(size_t i = 0; i < len; i++) {
        oneWire_write(instance, data[i]);
    }
}
bool unitemp_OneWire_alloc(void* s, uint16_t* anotherValues) {
    Sensor* sensor = (Sensor*)s;
    OneWireSensor* instance = malloc(sizeof(OneWireSensor));
    if(instance == NULL) {
        FURI_LOG_E(APP_NAME, "Sensor %s instance allocation error", sensor->name);
        return false;
    }
    sensor->instance = instance;

    instance->gpio = unitemp_GPIO_getFromInt(anotherValues[0]);
    if(instance->gpio != NULL) {
        return true;
    }
    FURI_LOG_E(APP_NAME, "Sensor %s GPIO setting error", sensor->name);
    free(instance);
    return false;
}

static uint8_t onewire_CRC_update(uint8_t crc, uint8_t b) {
    //  return pgm_read_byte(&onewire_crc_table[crc ^ b]);
    for(uint8_t p = 8; p; p--) {
        crc = ((crc ^ b) & 1) ? (crc >> 1) ^ 0b10001100 : (crc >> 1);
        b >>= 1;
    }
    return crc;
}

static bool onewire_CRC_check(uint8_t* data, size_t len) {
    uint8_t crc = 0;
    for(size_t i = 0; i < len; i++) {
        crc = onewire_CRC_update(crc, data[i]);
    }
    return !crc;
}

bool unitemp_OneWire_free(void* s) {
    Sensor* sensor = (Sensor*)s;
    free(sensor->instance);

    return true;
}

bool unitemp_OneWire_init(void* s) {
    OneWireSensor* instance = ((Sensor*)s)->instance;
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

    //Установка разрядности в 10 бит
    if(!oneWire_start(instance)) return false;
    oneWire_write(instance, 0xCC); // skip ROM
    oneWire_write(instance, 0x4E); // Запись в память
    uint8_t buff[3] = {0x4B, 0x46, 0x3F}; //10 бит
    oneWire_writeBytes(instance, buff, 3);

    //Сохранение значений в EEPROM для автоматического восстановления после сбоев питания
    if(!oneWire_start(instance)) return false;
    oneWire_write(instance, 0xCC); // skip ROM
    oneWire_write(instance, 0x48); // Запись в EEPROM

    return true;
}

bool unitemp_OneWire_deinit(void* s) {
    OneWireSensor* instance = ((Sensor*)s)->instance;
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

UnitempStatus unitemp_OneWire_update(void* s) {
    Sensor* sensor = (Sensor*)s;
    OneWireSensor* instance = ((Sensor*)s)->instance;
    if(sensor->status != UT_POLLING) {
        if(!oneWire_start(instance)) return UT_TIMEOUT;
        oneWire_write(instance, 0xCC); // skip ROM
        oneWire_write(instance, 0x44); // convert t
        return UT_POLLING;
    } else {
        if(!oneWire_start(instance)) return UT_TIMEOUT;
        oneWire_write(instance, 0xCC); // skip ROM
        oneWire_write(instance, 0xBE); // Read Scratch-pad
        uint8_t buff[9];
        oneWire_readBytes(instance, buff, 9);
        if(!onewire_CRC_check(buff, 9)) {
            return UT_BADCRC;
        }
        int16_t raw = buff[0] | ((int16_t)buff[1] << 8);
        sensor->temp = (float)raw / 16.0f;
    }

    return UT_OK;
}