// From https://github.com/DrZlo13/flipper-zero-adc-example

#pragma once
#include <stm32wbxx_ll_adc.h>
#include <stm32wbxx_ll_pwr.h>

typedef enum {
    FuriHalVref2048,
    FuriHalVref2500,
} FuriHalVref;

void furi_hal_adc_set_vref(FuriHalVref vref);

void furi_hal_adc_init();

void furi_hal_adc_enable();

void furi_hal_adc_disable();

void furi_hal_adc_deinit();

typedef enum {
    FuriHalAdcChannel0 = LL_ADC_CHANNEL_0,
    FuriHalAdcChannel1 = LL_ADC_CHANNEL_1,
    FuriHalAdcChannel2 = LL_ADC_CHANNEL_2,
    FuriHalAdcChannel3 = LL_ADC_CHANNEL_3,
    FuriHalAdcChannel4 = LL_ADC_CHANNEL_4,
    FuriHalAdcChannel5 = LL_ADC_CHANNEL_5,
    FuriHalAdcChannel6 = LL_ADC_CHANNEL_6,
    FuriHalAdcChannel7 = LL_ADC_CHANNEL_7,
    FuriHalAdcChannel8 = LL_ADC_CHANNEL_8,
    FuriHalAdcChannel9 = LL_ADC_CHANNEL_9,
    FuriHalAdcChannel10 = LL_ADC_CHANNEL_10,
    FuriHalAdcChannel11 = LL_ADC_CHANNEL_11,
    FuriHalAdcChannel12 = LL_ADC_CHANNEL_12,
    FuriHalAdcChannel13 = LL_ADC_CHANNEL_13,
    FuriHalAdcChannel14 = LL_ADC_CHANNEL_14,
    FuriHalAdcChannel15 = LL_ADC_CHANNEL_15,
    FuriHalAdcChannel16 = LL_ADC_CHANNEL_16,
    FuriHalAdcChannel17 = LL_ADC_CHANNEL_17,
    FuriHalAdcChannel18 = LL_ADC_CHANNEL_18,
    FuriHalAdcChannelVREFINT = LL_ADC_CHANNEL_VREFINT,
    FuriHalAdcChannelTEMPSENSOR = LL_ADC_CHANNEL_TEMPSENSOR,
    FuriHalAdcChannelVBAT = LL_ADC_CHANNEL_VBAT,
} FuriHalAdcChannel;

void furi_hal_adc_set_single_channel(FuriHalAdcChannel channel);

uint32_t furi_hal_adc_read_sw();
