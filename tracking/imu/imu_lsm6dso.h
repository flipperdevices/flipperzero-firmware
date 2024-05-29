#ifndef IMU_LSM6DSO_H   /* Include guard */
#define IMU_LSM6DSO_H

#include "../../lib/lsm6dso-api/lsm6dso_reg.h"

#include <furi_hal.h>

#include "imu.h"

#define LSM6DSO_TAG "LSM6DO-C"

#define LSM6DSO_DEV_ADDRESS UINT8_C(0xD6)

static const double LSM6DSO_DEG_TO_RAD = 0.017453292519943295769236907684886;

int32_t lsm6dso_write_i2c(void* handle, uint8_t reg_addr, uint8_t* data, uint16_t len);
int32_t lsm6dso_read_i2c(void* handle, uint8_t reg_addr, uint8_t* read_data, uint16_t len);

bool lsm6dso_begin();
void lsm6dso_end();
int lsm6dso_read(double* vec);

#endif