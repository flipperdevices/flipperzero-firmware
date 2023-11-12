#pragma once

#include <stddef.h>
#include <stdint.h>

#include "furi_hal_serial_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void furi_hal_serial_control_init(void);

void furi_hal_serial_control_deinit(void);

void furi_hal_serial_control_suspend(void);

void furi_hal_serial_control_resume(void);

FuriHalSerialHandle* furi_hal_serial_control_acquire(FuriHalUartId uart_id);

void furi_hal_serial_control_release(FuriHalSerialHandle* handle);

#ifdef __cplusplus
}
#endif
