#ifndef IMU_H
#define IMU_H

#include <stdbool.h>
#include <furi_hal.h>

#include "imu_t.h"
#include "bmi160.h"
#include "lsm6ds3trc.h"
#include "lsm6dso.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ACC_DATA_READY (1 << 0)
#define GYR_DATA_READY (1 << 1)

#define IMU_TAG "IMU_H"

static const double DEG_TO_RAD = 0.017453292519943295769236907684886;
static const double GRAVITY = 9.81;

bool imu_begin();
void imu_end();
int imu_read(double* vec);
struct imu_t* get_imu(uint8_t address);
unsigned int imu_scan_i2c();
struct imu_t* find_imu();

#ifdef __cplusplus
}
#endif
#endif // IMU_H
