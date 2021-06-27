#pragma once

#include <stdint.h>

typedef struct {
    const void (*on_start)(void* context);
    const void (*on_event)(void* context, uint32_t event);
    const void (*on_exit)(void* context);
} NfcScene;
