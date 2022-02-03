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

const SdUpdateProgress* sdcard_update_progress_get();

/* All progress update functions require display controller
 * & framebuffer to be inialized! */

void sdcard_update_progress_set_stage(const SdUpdateState state, const uint8_t progress_pct);
void sdcard_update_progress_update(const uint8_t progress_pct);
void sdcard_update_progress_render();
