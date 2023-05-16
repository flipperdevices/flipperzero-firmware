#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32wbxx.h"

typedef enum {
    FuriHalBusAHB1_GRP1,
    FuriHalBusDMA1,
    FuriHalBusDMA2,
    FuriHalBusDMAMUX1,
    FuriHalBusCRC,
    FuriHalBusTSC,

    FuriHalBusAHB2_GRP1,
    FuriHalBusGPIOA,
    FuriHalBusGPIOB,
    FuriHalBusGPIOC,
    FuriHalBusGPIOD,
    FuriHalBusGPIOE,
    FuriHalBusGPIOH,
#if defined(ADC_SUPPORT_5_MSPS)
    FuriHalBusADC,
#endif
    FuriHalBusAES1,

    FuriHalBusAHB3_GRP1,
    FuriHalBusQUADSPI,
    FuriHalBusPKA,
    FuriHalBusAES2,
    FuriHalBusRNG,
    FuriHalBusHSEM,
    FuriHalBusIPCC,
    FuriHalBusFLASH,

    FuriHalBusAPB1_GRP1,
    FuriHalBusTIM2,
    FuriHalBusLCD,
    FuriHalBusSPI2,
    FuriHalBusI2C1,
    FuriHalBusI2C3,
    FuriHalBusCRS,
    FuriHalBusUSB,
    FuriHalBusLPTIM1,

    FuriHalBusAPB1_GRP2,
    FuriHalBusLPUART1,
    FuriHalBusLPTIM2,

    FuriHalBusAPB2_GRP1,
#if defined(ADC_SUPPORT_2_5_MSPS)
    FuriHalBusADC,
#endif
    FuriHalBusTIM1,
    FuriHalBusSPI1,
    FuriHalBusUSART1,
    FuriHalBusTIM16,
    FuriHalBusTIM17,
    FuriHalBusSAI1,

    FuriHalBusAPB3_GRP1,
    FuriHalBusRF,

    FuriHalBusMAX,
} FuriHalBus;

/** Early initialization */
void furi_hal_bus_init_early();

/** Early de-initialization */
void furi_hal_bus_deinit_early();

/**
 * Enable a peripheral by turning the clocking on and deasserting the reset.
 * @param [in] bus Peripheral to be enabled.
 * @warning Peripheral must be in disabled state in order to be enabled.
 */
void furi_hal_bus_enable(FuriHalBus bus);

/**
 * Reset a peripheral by sequentially asserting and deasserting the reset.
 * @param [in] bus Peripheral to be reset.
 * @warning Peripheral must be in enabled state in order to be reset.
 */
void furi_hal_bus_reset(FuriHalBus bus);

/**
 * Disable a peripheral by turning the clocking off and asserting the reset.
 * @param [in] bus Peripheral to be disabled.
 * @warning Peripheral must be in enabled state in order to be disabled.
 */
void furi_hal_bus_disable(FuriHalBus bus);

#ifdef __cplusplus
}
#endif
