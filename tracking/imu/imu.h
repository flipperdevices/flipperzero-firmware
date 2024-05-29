#ifndef IMU_H
#define IMU_H

#include <stdbool.h>

#include <furi_hal.h>

#include "imu_bmi160.h"
#include "imu_lsm6ds3trc.h"
#include "imu_lsm6dso.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ACC_DATA_READY (1 << 0)
#define GYR_DATA_READY (1 << 1)

#define IMU_TAG "IMU_H"

bool imu_begin();
void imu_end();
int imu_read(double* vec);

#ifdef __cplusplus
}
#endif
#endif
