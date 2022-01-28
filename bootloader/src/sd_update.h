#pragma once

#include <stdint.h>

typedef enum {
    SdUpdateState_Unknown,
    SdUpdateState_Erase,
    SdUpdateState_Write,
    SdUpdateState_Verify,
    SdUpdateState_Complete,
    SdUpdateState_ErrorBase,
    SdUpdateState_Error_HwMismatch,
    SdUpdateState_Error_FlashError,
} SdUpdateState;

typedef struct {
    SdUpdateState operation;
    uint8_t operation_progress;
} update_status_detail;

void execute_sdcard_update();
