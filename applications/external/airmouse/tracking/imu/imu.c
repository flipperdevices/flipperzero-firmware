#include "imu.h"

#define IMU_TAG "IMU_H"

extern struct imu_t imu_bmi160;
extern struct imu_t imu_lsm6ds3trc;
extern struct imu_t imu_lsm6dso;

struct imu_t* imu_types[] = {&imu_bmi160, &imu_lsm6ds3trc, &imu_lsm6dso};

static const int imu_count = sizeof(imu_types) / sizeof(struct imu_t*);

static struct imu_t* imu_found;

struct imu_t* find_imu() {
    unsigned int i;
    for(i = 0; i < imu_count; i++) {
        if(furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, imu_types[i]->address, 50)) {
            FURI_LOG_E(IMU_TAG, "found i2c device address 0x%X", imu_types[i]->address);
            return imu_types[i];
        }
    }
    return NULL;
}

bool imu_begin() {
    bool ret = false;
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);

    if(imu_found == NULL) {
        imu_found = find_imu();
        if(imu_found != NULL) FURI_LOG_E(IMU_TAG, "Found Device %s", imu_found->name);
    }

    if(imu_found != NULL) ret = imu_found->begin();

    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return ret;
}

void imu_end() {
    if(imu_found == NULL) return;
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    imu_found->end();
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
}

int imu_read(double* vec) {
    if(imu_found == NULL) return 0;
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    int ret = imu_found->read(vec);
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return ret;
}
