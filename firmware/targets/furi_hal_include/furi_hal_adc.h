#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FuriHalAdcHandle FuriHalAdcHandle;

typedef enum {
    FuriHalVref2048,
    FuriHalVref2500,
} FuriHalVref;

void furi_hal_adc_init();

FuriHalAdcHandle* furi_hal_adc_aquire(size_t timeout);

void furi_hal_adc_release(FuriHalAdcHandle* handle);

#ifdef __cplusplus
}
#endif