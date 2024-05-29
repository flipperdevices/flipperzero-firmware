#ifndef IMU_BMI160_H
#define IMU_BMI160_H

#include "../../lib/bmi160-api/bmi160.h"
#include <furi_hal.h>
#include "imu.h"

// Define constants
#define BMI160_TAG "BMI160"
#define BMI160_DEV_ADDR (0x69 << 1)

// Function declarations
int8_t bmi160_write_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t len);
int8_t bmi160_read_i2c(uint8_t dev_addr, uint8_t reg_addr, uint8_t* read_data, uint16_t len);
bool bmi160_begin(void);
void bmi160_end(void);
int bmi160_read(double* vec);

#endif // IMU_BMI160_H
