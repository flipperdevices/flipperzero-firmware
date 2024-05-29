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

typedef struct
{
    unsigned int address;
    bool (*begin)(void);
    void (*end)(void);
    int (*read)(double* vec);
    char name[255];
} imu_t;

bool imu_begin();
void imu_end();
int imu_read(double* vec);
imu_t* get_imu(uint8_t address);
unsigned int imu_scan_i2c();
imu_t* find_imu();

#ifdef __cplusplus
}
#endif
#endif
