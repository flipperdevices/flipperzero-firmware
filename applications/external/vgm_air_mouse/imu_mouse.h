#pragma once

#include "sensors/ICM42688P.h"

typedef struct {
    bool (*mouse_move)(void* inst, int8_t dx, int8_t dy);
    bool (*mouse_key_press)(void* inst, uint8_t button);
    bool (*mouse_key_release)(void* inst, uint8_t button);
    bool (*mouse_scroll)(void* inst, int8_t value);
    uint32_t report_rate_max;
} ImuHidApi;

typedef enum {
    ImuMouseKeyRight,
    ImuMouseKeyLeft,
} ImuMouseKey;

typedef struct ImuThread ImuThread;

ImuThread* imu_start(ICM42688P* icm42688p, const ImuHidApi* hid, void* hid_inst);

void imu_stop(ImuThread* imu);

void imu_mouse_key_press(ImuThread* imu, ImuMouseKey key, bool state);

void imu_mouse_scroll_mode(ImuThread* imu, bool enable);
