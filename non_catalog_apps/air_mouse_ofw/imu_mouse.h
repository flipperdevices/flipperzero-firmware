#pragma once

#include "sensors/ICM42688P.h"

typedef enum {
    ImuMouseKeyRight,
    ImuMouseKeyLeft,
} ImuMouseKey;

typedef struct ImuThread ImuThread;

ImuThread* imu_start(ICM42688P* icm42688p);

void imu_stop(ImuThread* imu);

void imu_mouse_key_press(ImuThread* imu, ImuMouseKey key, bool state);
