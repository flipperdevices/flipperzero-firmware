#pragma once

#include <furi.h>

/**
 * UART channels
 */
typedef enum {
    FuriHalUartIdUSART1,
    FuriHalUartIdLPUART1,
    FuriHalUartIdMax,
} FuriHalUartId;

typedef struct {
    FuriHalUartId id;
    bool is_used;
} FuriHalSerialHandle;
