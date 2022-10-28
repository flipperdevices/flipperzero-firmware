#include "imu.h"
#include <furi_hal.h>

bool bmi160_begin();
int bmi160_read(double* vec);

bool lsm6ds3trc_begin();
void lsm6ds3trc_end();
int lsm6ds3trc_read(double* vec);

bool imu_begin() {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    bool ret = lsm6ds3trc_begin(); // bmi160_begin();
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return ret;
}

void imu_end() {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    lsm6ds3trc_end(); // 
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
}

int imu_read(double* vec) {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    int ret = lsm6ds3trc_read(vec); // bmi160_read(vec);
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return ret;
}
