#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <furi.h>
#include <furi_hal.h>

#include "preset.h"

typedef struct SubGhzDeviceRegistry SubGhzDeviceRegistry;
typedef struct SubGhzDevice SubGhzDevice;

typedef bool (*SubGhzBegin)(void);
typedef void (*SubGhzEnd)(void);
typedef void (*SubGhzReset)(void);
typedef void (*SubGhzSleep)(void);
typedef void (*SubGhzIdle)(void);
typedef void (*SubGhzLoadPreset)(FuriHalSubGhzPreset preset);
typedef uint32_t (*SubGhzSetFrequency)(uint32_t frequency);

typedef void (*SubGhzSetAsyncMirrorPin)(const GpioPin* gpio);

typedef bool (*SubGhzSetTx)(void);
typedef bool (*SubGhzStartAsyncTx)(void* callback, void* context);
typedef bool (*SubGhzIsAsyncCompleteTx)(void);
typedef void (*SubGhzStopAsyncTx)(void);

typedef void (*SubGhzSetRx)(void);
typedef void (*SubGhzStartAsyncRx)(void* callback, void* context);
typedef void (*SubGhzStopAsyncRx)(void);

typedef struct {
    SubGhzBegin begin;
    SubGhzEnd end;

    SubGhzReset reset;
    SubGhzSleep sleep;
    SubGhzIdle idle;

    SubGhzLoadPreset load_preset;
    SubGhzSetFrequency set_frequency;
    SubGhzSetAsyncMirrorPin set_async_mirror_pin;

    SubGhzSetTx set_tx;
    SubGhzStartAsyncTx start_async_tx;
    SubGhzIsAsyncCompleteTx is_async_complete_tx;
    SubGhzStopAsyncTx stop_async_tx;

    SubGhzSetRx set_rx;
    SubGhzStartAsyncRx start_async_rx;
    SubGhzStopAsyncRx stop_async_rx;
} SubGhzDeviceInterconnect;

struct SubGhzDevice {
    const char* name;
    const SubGhzDeviceInterconnect* interconnect;
};
