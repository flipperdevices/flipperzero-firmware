#ifndef IMU_LSM6DSO_H
#define IMU_LSM6DSO_H

#include "../../lib/lsm6dso-api/lsm6dso_reg.h"

#include <furi_hal.h>

#include "imu.h"

int32_t lsm6dso_write_i2c(void* handle, uint8_t reg_addr, uint8_t* data, uint16_t len);
int32_t lsm6dso_read_i2c(void* handle, uint8_t reg_addr, uint8_t* read_data, uint16_t len);

bool lsm6dso_begin();
void lsm6dso_end();
int lsm6dso_read(double* vec);

extern struct imu_t imu_lsm6dso;

#endif // IMU_LSM6DSO_H