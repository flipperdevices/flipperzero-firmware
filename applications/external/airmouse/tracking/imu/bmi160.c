#include "imu.h"
#include "../../lib/bmi160-api/bmi160.h"

#define BMI160_TAG "BMI160"
#define BMI160_DEV_ADDR (0x69 << 1)

struct bmi160_dev bmi160dev;
struct bmi160_sensor_data bmi160_accel;
struct bmi160_sensor_data bmi160_gyro;

int8_t bmi160_write_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t len) {
    if(furi_hal_i2c_write_mem(&furi_hal_i2c_handle_external, dev_addr, reg_addr, data, len, 50))
        return BMI160_OK;
    return BMI160_E_COM_FAIL;
}

int8_t bmi160_read_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t* read_data, uint16_t len) {
    if(furi_hal_i2c_read_mem(&furi_hal_i2c_handle_external, dev_addr, reg_addr, read_data, len, 50))
        return BMI160_OK;
    return BMI160_E_COM_FAIL;
}

bool bmi160_begin() {
    FURI_LOG_I(BMI160_TAG, "Init BMI160");

    if(!furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, BMI160_DEV_ADDR, 50)) {
        FURI_LOG_E(BMI160_TAG, "Device not ready!");
        return false;
    }

    FURI_LOG_I(BMI160_TAG, "Device ready!");

    bmi160dev.id = BMI160_DEV_ADDR;
    bmi160dev.intf = BMI160_I2C_INTF;
    bmi160dev.read = bmi160_read_i2c;
    bmi160dev.write = bmi160_write_i2c;
    bmi160dev.delay_ms = furi_delay_ms;

    if(bmi160_init(&bmi160dev) != BMI160_OK) {
        FURI_LOG_E(BMI160_TAG, "Initialization failure!");
        FURI_LOG_E(BMI160_TAG, "Chip ID 0x%X", bmi160dev.chip_id);
        return false;
    }

    bmi160dev.accel_cfg.odr = BMI160_ACCEL_ODR_400HZ;
    bmi160dev.accel_cfg.range = BMI160_ACCEL_RANGE_4G;
    bmi160dev.accel_cfg.bw = BMI160_ACCEL_BW_NORMAL_AVG4;
    bmi160dev.accel_cfg.power = BMI160_ACCEL_NORMAL_MODE;
    bmi160dev.gyro_cfg.odr = BMI160_GYRO_ODR_400HZ;
    bmi160dev.gyro_cfg.range = BMI160_GYRO_RANGE_2000_DPS;
    bmi160dev.gyro_cfg.bw = BMI160_GYRO_BW_NORMAL_MODE;
    bmi160dev.gyro_cfg.power = BMI160_GYRO_NORMAL_MODE;

    if(bmi160_set_sens_conf(&bmi160dev) != BMI160_OK) {
        FURI_LOG_E(BMI160_TAG, "Initialization failure!");
        FURI_LOG_E(BMI160_TAG, "Chip ID 0x%X", bmi160dev.chip_id);
        return false;
    }

    FURI_LOG_I(BMI160_TAG, "Initialization success!");
    FURI_LOG_I(BMI160_TAG, "Chip ID 0x%X", bmi160dev.chip_id);

    return true;
}

int bmi160_read(double* vec) {
    if(bmi160_get_sensor_data(
           (BMI160_ACCEL_SEL | BMI160_GYRO_SEL), &bmi160_accel, &bmi160_gyro, &bmi160dev) !=
       BMI160_OK) {
        return 0;
    }

    vec[0] = ((double)bmi160_accel.x * 4 / 32768) * GRAVITY;
    vec[1] = ((double)bmi160_accel.y * 4 / 32768) * GRAVITY;
    vec[2] = ((double)bmi160_accel.z * 4 / 32768) * GRAVITY;
    vec[3] = ((double)bmi160_gyro.x * 2000 / 32768) * DEG_TO_RAD;
    vec[4] = ((double)bmi160_gyro.y * 2000 / 32768) * DEG_TO_RAD;
    vec[5] = ((double)bmi160_gyro.z * 2000 / 32768) * DEG_TO_RAD;

    return ACC_DATA_READY | GYR_DATA_READY;
}

void bmi160_end() {
}

struct imu_t imu_bmi160 = {BMI160_DEV_ADDR, bmi160_begin, bmi160_end, bmi160_read, BMI160_TAG};
