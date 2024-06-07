#ifndef IMU_H
#define IMU_H

#include <stdbool.h>
#include <furi_hal.h>

#ifdef __cplusplus
extern "C" {
#endif

struct imu_t {
    unsigned int address;
    bool (*begin)(void);
    void (*end)(void);
    int (*read)(double* vec);
    char* name;
};

#define ACC_DATA_READY (1 << 0)
#define GYR_DATA_READY (1 << 1)

static const double DEG_TO_RAD = 0.017453292519943295769236907684886;
static const double GRAVITY = 9.81;

bool imu_begin();
void imu_end();
int imu_read(double* vec);

#ifdef __cplusplus
}
#endif
#endif // IMU_H
