// From https://github.com/DrZlo13/flipper-zero-adc-example

#pragma once
#include <stm32wbxx_ll_adc.h>
#include <stm32wbxx_ll_pwr.h>
#include <furi_hal_adc.h>

typedef enum {
    FuriHalVref2048,
    FuriHalVref2500,
} FuriHalVref;

void furi_hal_adc_set_vref(FuriHalVref vref);

void _furi_hal_adc_init(void);

void furi_hal_adc_enable(void);

void furi_hal_adc_disable(void);

void furi_hal_adc_deinit(void);

void furi_hal_adc_set_single_channel(FuriHalAdcChannel channel);

uint32_t furi_hal_adc_read_sw(void);
