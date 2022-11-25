#include "BMP280.h"

const SensorType BMP280 = {
    .typename = "BMP280",
    .interface = &I2C,
    .pollingInterval = 500,
    .allocator = unitemp_BMP280_alloc,
    .mem_releaser = unitemp_BMP280_free,
    .initializer = unitemp_BMP280_init,
    .deinitializer = unitemp_BMP280_deinit,
    .updater = unitemp_BMP280_update};

//Интервал обновления калибровочных значений
#define BMP280_CAL_UPDATE_INTERVAL 60000

#define TEMP_CAL_START_ADDR 0x88
#define PRESS_CAL_START_ADDR 0x8E
#define BMP280_ID 0x58

#define BMP280_REG_STATUS 0xF3
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG 0xF5
//Преддескретизация температуры
#define BMP280_TEMP_OVERSAMPLING_SKIP 0b00000000
#define BMP280_TEMP_OVERSAMPLING_1 0b00100000
#define BMP280_TEMP_OVERSAMPLING_2 0b01000000
#define BMP280_TEMP_OVERSAMPLING_4 0b01100000
#define BMP280_TEMP_OVERSAMPLING_8 0b10000000
#define BMP280_TEMP_OVERSAMPLING_16 0b10100000
//Преддескретизация давления
#define BMP280_PRESS_OVERSAMPLING_SKIP 0b00000000
#define BMP280_PRESS_OVERSAMPLING_1 0b00000100
#define BMP280_PRESS_OVERSAMPLING_2 0b00001000
#define BMP280_PRESS_OVERSAMPLING_4 0b00001100
#define BMP280_PRESS_OVERSAMPLING_8 0b00010000
#define BMP280_PRESS_OVERSAMPLING_16 0b00010100
//Режимы работы датчика
#define BMP280_MODE_SLEEP 0b00000000 //Спит и мало кушает
#define BMP280_MODE_FORCED 0b00000001 //Обновляет значения 1 раз, после чего уходит в сон
#define BMP280_MODE_NORMAL 0b00000011 //Регулярно обновляет значения
//Период обновления в нормальном режиме
#define BMP280_STANDBY_TIME_0_5 0b00000000
#define BMP280_STANDBY_TIME_62_5 0b00100000
#define BMP280_STANDBY_TIME_125 0b01000000
#define BMP280_STANDBY_TIME_250 0b01100000
#define BMP280_STANDBY_TIME_500 0b10000000
#define BMP280_STANDBY_TIME_1000 0b10100000
#define BMP280_STANDBY_TIME_2000 0b11000000
#define BMP280_STANDBY_TIME_4000 0b11100000
//Коэффициент фильтрации значений
#define BMP280_FILTER_COEFF_1 0b00000000
#define BMP280_FILTER_COEFF_2 0b00000100
#define BMP280_FILTER_COEFF_4 0b00001000
#define BMP280_FILTER_COEFF_8 0b00001100
#define BMP280_FILTER_COEFF_16 0b00010000
//Разрешить работу по SPI
#define BMP280_SPI_3W_ENABLE 0b00000001
#define BMP280_SPI_3W_DISABLE 0b00000000

static double bmp280_compensate_T_double(I2CSensor* i2c_sensor, int32_t adc_T) {
    BMP280_instance* bmp280_instance = (BMP280_instance*)i2c_sensor->sensorInstance;
    double var1, var2, T;
    var1 = (((double)adc_T) / (double)16384.0 -
            ((double)bmp280_instance->temp_cal.dig_T1) / (double)1024.0) *
           ((double)bmp280_instance->temp_cal.dig_T2);
    var2 = ((((double)adc_T) / (double)131072.0 -
             ((double)bmp280_instance->temp_cal.dig_T1) / (double)8192.0) *
            (((double)adc_T) / (double)131072.0 -
             ((double)bmp280_instance->temp_cal.dig_T1) / (double)8192.0)) *
           ((double)bmp280_instance->temp_cal.dig_T3);
    bmp280_instance->t_fine = var1 + var2;
    T = (var1 + var2) / (double)5120.0;
    return T;
}

static double bmp280_compensate_P_double(I2CSensor* i2c_sensor, int32_t adc_P) {
    BMP280_instance* bmp280_instance = (BMP280_instance*)i2c_sensor->sensorInstance;
    // double var1, var2, p;
    // var1 = ((double)bmp280_instance->t_fine / (double)2.0) - (double)64000.0;
    // var2 = var1 * var1 * ((double)bmp280_instance->press_cal.dig_P6) / (double)32768.0;
    // var2 = var2 + var1 * ((double)bmp280_instance->press_cal.dig_P5) * (double)2.0;
    // var2 = (var2 / (double)4.0) + (((double)bmp280_instance->press_cal.dig_P4) * (double)65536.0);
    // var1 = (((double)bmp280_instance->press_cal.dig_P3) * var1 * var1 / (double)524288.0 +
    //         ((double)bmp280_instance->press_cal.dig_P2) * var1) /
    //        (double)524288.0;
    // var1 = ((double)1.0 + var1 / (double)32768.0) * ((double)bmp280_instance->press_cal.dig_P1);
    // if(var1 == (double)0.0) {
    //     return 0; // avoid exception caused by division by zero
    // }
    // p = (double)1048576.0 - (double)adc_P;
    // p = (p - (var2 / (double)4096.0)) * (double)6250.0 / var1;
    // var1 = ((double)bmp280_instance->press_cal.dig_P9) * p * p / (double)2147483648.0;
    // var2 = p * ((double)bmp280_instance->press_cal.dig_P8) / (double)32768.0;
    // p = p + (var1 + var2 + ((double)bmp280_instance->press_cal.dig_P7)) / (double)16.0;
    int64_t value_1 = (bmp280_instance->t_fine) - 128000;
    int64_t value_2 = value_1 * value_1 * (int64_t)bmp280_instance->press_cal.dig_P6;
    value_2 = value_2 + ((value_1 * (int64_t)bmp280_instance->press_cal.dig_P5) << 17);
    value_2 = value_2 + (((int64_t)bmp280_instance->press_cal.dig_P4) << 35);
    value_1 = ((value_1 * value_1 * (int64_t)bmp280_instance->press_cal.dig_P3) >> 8) +
              ((value_1 * (int64_t)bmp280_instance->press_cal.dig_P2) << 12);
    value_1 = (((((int64_t)1) << 47) + value_1)) * ((int64_t)bmp280_instance->press_cal.dig_P1) >>
              33;

    if(!value_1) return 0; // Avoid division by zero

    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - value_2) * 3125) / value_1;
    value_1 = (((int64_t)bmp280_instance->press_cal.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    value_2 = (((int64_t)bmp280_instance->press_cal.dig_P8) * p) >> 19;
    p = ((p + value_1 + value_2) >> 8) + (((int64_t)bmp280_instance->press_cal.dig_P7) << 4);
    return p / 256.0;
}

static bool bmp280_readCalValues(I2CSensor* i2c_sensor) {
    BMP280_instance* bmp280_instance = (BMP280_instance*)i2c_sensor->sensorInstance;
    if(!unitemp_i2c_readRegArray(
           i2c_sensor, TEMP_CAL_START_ADDR, 6, (uint8_t*)&bmp280_instance->temp_cal))
        return false;
    FURI_LOG_D(
        APP_NAME,
        "Sensor BMP280 (0x%02X) calibration values: T1: %d, T2: %d, T3: %d",
        i2c_sensor->currentI2CAdr,
        bmp280_instance->temp_cal.dig_T1,
        bmp280_instance->temp_cal.dig_T2,
        bmp280_instance->temp_cal.dig_T3);
    if(!unitemp_i2c_readRegArray(
           i2c_sensor, PRESS_CAL_START_ADDR, 18, (uint8_t*)&bmp280_instance->press_cal))
        return false;
    FURI_LOG_D(
        APP_NAME,
        "Sensor BMP280 (0x%02X): P1-9: %d, %d, %d, %d, %d, %d, %d, %d, %d",
        i2c_sensor->currentI2CAdr,
        bmp280_instance->press_cal.dig_P1,
        bmp280_instance->press_cal.dig_P2,
        bmp280_instance->press_cal.dig_P3,
        bmp280_instance->press_cal.dig_P4,
        bmp280_instance->press_cal.dig_P5,
        bmp280_instance->press_cal.dig_P6,
        bmp280_instance->press_cal.dig_P7,
        bmp280_instance->press_cal.dig_P8,
        bmp280_instance->press_cal.dig_P9);
    bmp280_instance->last_cal_update_time = furi_get_tick();
    return true;
}
static bool bmp280_isMeasuring(Sensor* sensor) {
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;
    return (bool)((unitemp_i2c_readReg(i2c_sensor, BMP280_REG_STATUS) & 0x08) >> 3);
}

bool unitemp_BMP280_alloc(Sensor* sensor, char* args) {
    UNUSED(args);
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;
    BMP280_instance* bmp280_instance = malloc(sizeof(BMP280_instance));
    if(bmp280_instance == NULL) {
        FURI_LOG_E(APP_NAME, "Failed to allocation sensor %s instance", sensor->name);
        return false;
    }
    i2c_sensor->sensorInstance = bmp280_instance;

    i2c_sensor->minI2CAdr = 0x76;
    i2c_sensor->maxI2CAdr = 0x77;
    return true;
}

bool unitemp_BMP280_init(Sensor* sensor) {
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;
    //Перезагрузка
    unitemp_i2c_writeReg(i2c_sensor, 0xE0, 0xB6);
    //Чтение ID датчика
    uint8_t id = unitemp_i2c_readReg(i2c_sensor, 0xD0);
    if(id != BMP280_ID) {
        FURI_LOG_E(
            APP_NAME,
            "Sensor %s returned wrong ID 0x%02X, expected 0x%02X",
            sensor->name,
            id,
            BMP280_ID);
        return false;
    }

    //Чтение калибровочных значений
    if(!bmp280_readCalValues(i2c_sensor)) {
        FURI_LOG_E(APP_NAME, "Failed to read calibration values sensor %s", sensor->name);
        return false;
    }
    //Настройка режимов работы
    unitemp_i2c_writeReg(
        i2c_sensor,
        BMP280_REG_CTRL_MEAS,
        BMP280_TEMP_OVERSAMPLING_2 | BMP280_PRESS_OVERSAMPLING_4 | BMP280_MODE_NORMAL);
    //Настройка периода опроса и фильтрации значений
    unitemp_i2c_writeReg(
        i2c_sensor,
        BMP280_REG_CONFIG,
        BMP280_STANDBY_TIME_500 | BMP280_FILTER_COEFF_16 | BMP280_SPI_3W_DISABLE);

    return true;
}

bool unitemp_BMP280_deinit(Sensor* sensor) {
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;
    //Перевод в сон
    unitemp_i2c_writeReg(i2c_sensor, BMP280_REG_CTRL_MEAS, BMP280_MODE_SLEEP);
    return true;
}

UnitempStatus unitemp_BMP280_update(Sensor* sensor) {
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;
    BMP280_instance* instance = i2c_sensor->sensorInstance;

    uint32_t t = furi_get_tick();

    if(furi_get_tick() - instance->last_cal_update_time > BMP280_CAL_UPDATE_INTERVAL) {
        bmp280_readCalValues(i2c_sensor);
    }

    while(bmp280_isMeasuring(sensor)) {
        if(furi_get_tick() - t > 100) {
            return UT_TIMEOUT;
        }
    }

    uint8_t buff[3];
    if(!unitemp_i2c_readRegArray(i2c_sensor, 0xFA, 3, buff)) return UT_TIMEOUT;
    int32_t adc_T = ((int32_t)buff[0] << 12) | ((int32_t)buff[1] << 4) | ((int32_t)buff[2] >> 4);
    if(!unitemp_i2c_readRegArray(i2c_sensor, 0xF7, 3, buff)) return UT_TIMEOUT;
    int32_t adc_P = ((int32_t)buff[0] << 12) | ((int32_t)buff[1] << 4) | ((int32_t)buff[2] >> 4);
    sensor->temp = bmp280_compensate_T_double(i2c_sensor, adc_T);
    sensor->pressure = bmp280_compensate_P_double(i2c_sensor, adc_P);
    FURI_LOG_D(APP_NAME, "pressure: %d pa ", (int)sensor->pressure);
    return UT_OK;
}

bool unitemp_BMP280_free(Sensor* sensor) {
    I2CSensor* i2c_sensor = (I2CSensor*)sensor->instance;
    free(i2c_sensor->sensorInstance);
    return true;
}