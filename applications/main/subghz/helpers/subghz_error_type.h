#pragma once

#include <furi.h>
#include <furi_hal.h>

/** SubGhzErrorType */
typedef enum {
    SubGhzErrorTypeNoError=0,
    SubGhzErrorTypeParseFile,
    SubGhzErrorTypeOnlyRX,
} SubGhzErrorType;

