#include "imu.h"

imu_t imu_types[] = {
    {
        BMI160_DEV_ADDR,
        bmi160_begin,
        bmi160_end,
        bmi160_read,
        "BMI160"
    },
    {
        LSM6DS3_ADDRESS,
        lsm6ds3trc_begin,
        lsm6ds3trc_end,
        lsm6ds3trc_read,
        "LSM6DS3"
    },
    {
        LSM6DSO_DEV_ADDRESS,
        lsm6dso_begin,
        lsm6dso_end,
        lsm6dso_read,
        "LSM6DSO"
    }
};

imu_t* imu_found;

bool imu_begin() {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    if (imu_found == NULL) {
        imu_found = find_imu();
        FURI_LOG_E(IMU_TAG, "Found Device %s", imu_found->name);
    }
    bool ret = 0;
    if (imu_found != NULL) {
        ret = imu_found->begin();
    }
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return ret;
}

void imu_end() {
    if (imu_found == NULL) return;
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    imu_found->end();
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
}

int imu_read(double* vec) {
    if (imu_found == NULL) return 0;
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    int ret = imu_found->read(vec);
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return ret;
}

imu_t* get_imu(uint8_t address) {
    unsigned int i;
    for(i = 0; i < 3; i++) {
        if (imu_types[i].address == address) {
            return &imu_types[i];
        }
    }
    return NULL;
}

/**
 * Gives the first found i2c address, there should be only one device connected.
**/
unsigned int imu_scan_i2c() {
    unsigned int address;
    for(address = 1; address < 0xff; address++) {
        if(furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, address, 50)) {
            FURI_LOG_E(IMU_TAG, "found i2c device address 0x%X", address);
            return address;
        }
    }
    return 0;
}

imu_t* find_imu() {
   unsigned int address = imu_scan_i2c();
   return get_imu(address);
}