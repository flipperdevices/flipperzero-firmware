#include "imu.h"

bool bmi160_begin();
int bmi160_read(double* vec);

bool lsm6ds3trc_begin();
void lsm6ds3trc_end();
int lsm6ds3trc_read(double* vec);

bool imu_begin() {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    bool ret = lsm6dso_begin(); // lsm6ds3trc_begin();
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return ret;
}

void imu_end() {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    lsm6dso_end();
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
}

int imu_read(double* vec) {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    int ret = lsm6dso_read(vec); // lsm6ds3trc_read(vec);
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return ret;
}
