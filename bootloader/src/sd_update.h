#pragma once

#include <stdint.h>

typedef enum {
    SdUpdateState_Unknown,
    SdUpdateState_Mount,
    SdUpdateState_Open,
    SdUpdateState_ValidateImage,
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
    uint8_t progress, rendered_progress;
} SdUpdateProgress;

void sdcard_update_execute();
