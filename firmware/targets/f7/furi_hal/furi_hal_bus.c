#include <furi_hal_bus.h>
#include <furi.h>

#include <stm32wbxx_ll_bus.h>

#define FURI_HAL_BUS_IGNORE (0x0U)

#define FURI_HAL_BUS_AHB1_GRP1                                                           \
    (LL_AHB1_GRP1_PERIPH_DMA1 | LL_AHB1_GRP1_PERIPH_DMA2 | LL_AHB1_GRP1_PERIPH_DMAMUX1 | \
     LL_AHB1_GRP1_PERIPH_CRC | LL_AHB1_GRP1_PERIPH_TSC)

#if defined(ADC_SUPPORT_5_MSPS)
#define FURI_HAL_BUS_AHB2_GRP1                                                           \
    (LL_AHB2_GRP1_PERIPH_GPIOA | LL_AHB2_GRP1_PERIPH_GPIOB | LL_AHB2_GRP1_PERIPH_GPIOC | \
     LL_AHB2_GRP1_PERIPH_GPIOD | LL_AHB2_GRP1_PERIPH_GPIOE | LL_AHB2_GRP1_PERIPH_GPIOH | \
     LL_AHB2_GRP1_PERIPH_ADC | LL_AHB2_GRP1_PERIPH_AES1)

#define FURI_HAL_BUS_APB2_GRP1                                                          \
    (LL_APB2_GRP1_PERIPH_TIM1 | LL_APB2_GRP1_PERIPH_SPI1 | LL_APB2_GRP1_PERIPH_USART1 | \
     LL_APB2_GRP1_PERIPH_TIM16 | LL_APB2_GRP1_PERIPH_TIM17 | LL_APB2_GRP1_PERIPH_SAI1)
#else
#define FURI_HAL_BUS_AHB2_GRP1                                                           \
    (LL_AHB2_GRP1_PERIPH_GPIOA | LL_AHB2_GRP1_PERIPH_GPIOB | LL_AHB2_GRP1_PERIPH_GPIOC | \
     LL_AHB2_GRP1_PERIPH_GPIOD | LL_AHB2_GRP1_PERIPH_GPIOE | LL_AHB2_GRP1_PERIPH_GPIOH | \
     LL_AHB2_GRP1_PERIPH_AES1)

#define FURI_HAL_BUS_APB2_GRP1                                                            \
    (LL_APB2_GRP1_PERIPH_ADC | LL_APB2_GRP1_PERIPH_TIM1 | LL_APB2_GRP1_PERIPH_SPI1 |      \
     LL_APB2_GRP1_PERIPH_USART1 | LL_APB2_GRP1_PERIPH_TIM16 | LL_APB2_GRP1_PERIPH_TIM17 | \
     LL_APB2_GRP1_PERIPH_SAI1)
#endif

#define FURI_HAL_BUS_AHB3_GRP1                                                          \
    (LL_AHB3_GRP1_PERIPH_QUADSPI | LL_AHB3_GRP1_PERIPH_PKA | LL_AHB3_GRP1_PERIPH_AES2 | \
     LL_AHB3_GRP1_PERIPH_RNG | LL_AHB3_GRP1_PERIPH_HSEM | LL_AHB3_GRP1_PERIPH_IPCC)
//   LL_AHB3_GRP1_PERIPH_FLASH enabled by default

#define FURI_HAL_BUS_APB1_GRP1                                                       \
    (LL_APB1_GRP1_PERIPH_TIM2 | LL_APB1_GRP1_PERIPH_LCD | LL_APB1_GRP1_PERIPH_SPI2 | \
     LL_APB1_GRP1_PERIPH_I2C1 | LL_APB1_GRP1_PERIPH_I2C3 | LL_APB1_GRP1_PERIPH_CRS | \
     LL_APB1_GRP1_PERIPH_USB | LL_APB1_GRP1_PERIPH_LPTIM1)

#define FURI_HAL_BUS_APB1_GRP2 (LL_APB1_GRP2_PERIPH_LPUART1 | LL_APB1_GRP2_PERIPH_LPTIM2)

static const uint32_t furi_hal_bus[] = {
    [FuriHalBusAHB1_GRP1] = FURI_HAL_BUS_AHB1_GRP1,
    [FuriHalBusDMA1] = LL_AHB1_GRP1_PERIPH_DMA1,
    [FuriHalBusDMA2] = LL_AHB1_GRP1_PERIPH_DMA2,
    [FuriHalBusDMAMUX1] = LL_AHB1_GRP1_PERIPH_DMAMUX1,
    [FuriHalBusCRC] = LL_AHB1_GRP1_PERIPH_CRC,
    [FuriHalBusTSC] = LL_AHB1_GRP1_PERIPH_TSC,

    [FuriHalBusAHB2_GRP1] = FURI_HAL_BUS_AHB2_GRP1,
    [FuriHalBusGPIOA] = LL_AHB2_GRP1_PERIPH_GPIOA,
    [FuriHalBusGPIOB] = LL_AHB2_GRP1_PERIPH_GPIOB,
    [FuriHalBusGPIOC] = LL_AHB2_GRP1_PERIPH_GPIOC,
    [FuriHalBusGPIOD] = LL_AHB2_GRP1_PERIPH_GPIOD,
    [FuriHalBusGPIOE] = LL_AHB2_GRP1_PERIPH_GPIOE,
    [FuriHalBusGPIOH] = LL_AHB2_GRP1_PERIPH_GPIOH,
#if defined(ADC_SUPPORT_5_MSPS)
    [FuriHalBusADC] = LL_AHB2_GRP1_PERIPH_ADC,
#endif
    [FuriHalBusAES1] = LL_AHB2_GRP1_PERIPH_AES1,

    [FuriHalBusAHB3_GRP1] = FURI_HAL_BUS_AHB3_GRP1,
    [FuriHalBusQUADSPI] = LL_AHB3_GRP1_PERIPH_QUADSPI,
    [FuriHalBusPKA] = LL_AHB3_GRP1_PERIPH_PKA,
    [FuriHalBusAES2] = LL_AHB3_GRP1_PERIPH_AES2,
    [FuriHalBusRNG] = LL_AHB3_GRP1_PERIPH_RNG,
    [FuriHalBusHSEM] = LL_AHB3_GRP1_PERIPH_HSEM,
    [FuriHalBusIPCC] = LL_AHB3_GRP1_PERIPH_IPCC,
    [FuriHalBusFLASH] = LL_AHB3_GRP1_PERIPH_FLASH,

    [FuriHalBusAPB1_GRP1] = FURI_HAL_BUS_APB1_GRP1,
    [FuriHalBusTIM2] = LL_APB1_GRP1_PERIPH_TIM2,
    [FuriHalBusLCD] = LL_APB1_GRP1_PERIPH_LCD,
    [FuriHalBusSPI2] = LL_APB1_GRP1_PERIPH_SPI2,
    [FuriHalBusI2C1] = LL_APB1_GRP1_PERIPH_I2C1,
    [FuriHalBusI2C3] = LL_APB1_GRP1_PERIPH_I2C3,
    [FuriHalBusCRS] = LL_APB1_GRP1_PERIPH_CRS,
    [FuriHalBusUSB] = LL_APB1_GRP1_PERIPH_USB,
    [FuriHalBusLPTIM1] = LL_APB1_GRP1_PERIPH_LPTIM1,

    [FuriHalBusAPB1_GRP2] = FURI_HAL_BUS_APB1_GRP2,
    [FuriHalBusLPUART1] = LL_APB1_GRP2_PERIPH_LPUART1,
    [FuriHalBusLPTIM2] = LL_APB1_GRP2_PERIPH_LPTIM2,

    [FuriHalBusAPB2_GRP1] = FURI_HAL_BUS_APB2_GRP1,
#if defined(ADC_SUPPORT_2_5_MSPS)
    [FuriHalBusADC] = LL_APB2_GRP1_PERIPH_ADC,
#endif
    [FuriHalBusTIM1] = LL_APB2_GRP1_PERIPH_TIM1,
    [FuriHalBusSPI1] = LL_APB2_GRP1_PERIPH_SPI1,
    [FuriHalBusUSART1] = LL_APB2_GRP1_PERIPH_USART1,
    [FuriHalBusTIM16] = LL_APB2_GRP1_PERIPH_TIM16,
    [FuriHalBusTIM17] = LL_APB2_GRP1_PERIPH_TIM17,
    [FuriHalBusSAI1] = LL_APB2_GRP1_PERIPH_SAI1,

    [FuriHalBusAPB3_GRP1] = FURI_HAL_BUS_IGNORE, // APB3_GRP1 clocking cannot be changed
    [FuriHalBusRF] = LL_APB3_GRP1_PERIPH_RF,
};

void furi_hal_bus_init_early() {
    furi_hal_bus_disable(FuriHalBusAHB1_GRP1);
    furi_hal_bus_disable(FuriHalBusAHB2_GRP1);
    furi_hal_bus_disable(FuriHalBusAHB3_GRP1);
    furi_hal_bus_disable(FuriHalBusAPB1_GRP1);
    furi_hal_bus_disable(FuriHalBusAPB1_GRP2);
    furi_hal_bus_disable(FuriHalBusAPB2_GRP1);
    furi_hal_bus_disable(FuriHalBusAPB3_GRP1);
}

void furi_hal_bus_deinit_early() {
    furi_hal_bus_enable(FuriHalBusAHB1_GRP1);
    furi_hal_bus_enable(FuriHalBusAHB2_GRP1);
    furi_hal_bus_enable(FuriHalBusAHB3_GRP1);
    furi_hal_bus_enable(FuriHalBusAPB1_GRP1);
    furi_hal_bus_enable(FuriHalBusAPB1_GRP2);
    furi_hal_bus_enable(FuriHalBusAPB2_GRP1);
    furi_hal_bus_enable(FuriHalBusAPB3_GRP1);
}

void furi_hal_bus_enable(FuriHalBus bus) {
    furi_check(bus < FuriHalBusMAX);
    uint32_t value = furi_hal_bus[bus];
    if(!value) {
        return;
    }

    FURI_CRITICAL_ENTER();
    if(bus < FuriHalBusAHB2_GRP1) {
        furi_check(!LL_AHB1_GRP1_IsEnabledClock(value));
        LL_AHB1_GRP1_EnableClock(value);
        LL_AHB1_GRP1_ReleaseReset(value);
    } else if(bus < FuriHalBusAHB3_GRP1) {
        furi_check(!LL_AHB2_GRP1_IsEnabledClock(value));
        LL_AHB2_GRP1_EnableClock(value);
        LL_AHB2_GRP1_ReleaseReset(value);
    } else if(bus < FuriHalBusAPB1_GRP1) {
        furi_check(!LL_AHB3_GRP1_IsEnabledClock(value));
        LL_AHB3_GRP1_EnableClock(value);
        LL_AHB3_GRP1_ReleaseReset(value);
    } else if(bus < FuriHalBusAPB1_GRP2) {
        furi_check(!LL_APB1_GRP1_IsEnabledClock(value));
        LL_APB1_GRP1_EnableClock(value);
        LL_APB1_GRP1_ReleaseReset(value);
    } else if(bus < FuriHalBusAPB2_GRP1) {
        furi_check(!LL_APB1_GRP2_IsEnabledClock(value));
        LL_APB1_GRP2_EnableClock(value);
        LL_APB1_GRP2_ReleaseReset(value);
    } else if(bus < FuriHalBusAPB3_GRP1) {
        furi_check(!LL_APB2_GRP1_IsEnabledClock(value));
        LL_APB2_GRP1_EnableClock(value);
        LL_APB2_GRP1_ReleaseReset(value);
    } else {
        LL_APB3_GRP1_ReleaseReset(value);
    }
    FURI_CRITICAL_EXIT();
}

void furi_hal_bus_reset(FuriHalBus bus) {
    furi_check(bus < FuriHalBusMAX);
    uint32_t value = furi_hal_bus[bus];
    if(!value) {
        return;
    }

    FURI_CRITICAL_ENTER();
    if(bus < FuriHalBusAHB2_GRP1) {
        LL_AHB1_GRP1_ForceReset(value);
        LL_AHB1_GRP1_ReleaseReset(value);
    } else if(bus < FuriHalBusAHB3_GRP1) {
        LL_AHB2_GRP1_ForceReset(value);
        LL_AHB2_GRP1_ReleaseReset(value);
    } else if(bus < FuriHalBusAPB1_GRP1) {
        LL_AHB3_GRP1_ForceReset(value);
        LL_AHB3_GRP1_ReleaseReset(value);
    } else if(bus < FuriHalBusAPB1_GRP2) {
        LL_APB1_GRP1_ForceReset(value);
        LL_APB1_GRP1_ReleaseReset(value);
    } else if(bus < FuriHalBusAPB2_GRP1) {
        LL_APB1_GRP2_ForceReset(value);
        LL_APB1_GRP2_ReleaseReset(value);
    } else if(bus < FuriHalBusAPB3_GRP1) {
        LL_APB2_GRP1_ForceReset(value);
        LL_APB2_GRP1_ReleaseReset(value);
    } else {
        LL_APB3_GRP1_ForceReset(value);
        LL_APB3_GRP1_ReleaseReset(value);
    }
    FURI_CRITICAL_EXIT();
}

void furi_hal_bus_disable(FuriHalBus bus) {
    furi_check(bus < FuriHalBusMAX);
    uint32_t value = furi_hal_bus[bus];
    if(!value) {
        return;
    }

    FURI_CRITICAL_ENTER();
    if(bus < FuriHalBusAHB2_GRP1) {
        LL_AHB1_GRP1_ForceReset(value);
        LL_AHB1_GRP1_DisableClock(value);
    } else if(bus < FuriHalBusAHB3_GRP1) {
        LL_AHB2_GRP1_ForceReset(value);
        LL_AHB2_GRP1_DisableClock(value);
    } else if(bus < FuriHalBusAPB1_GRP1) {
        LL_AHB3_GRP1_ForceReset(value);
        LL_AHB3_GRP1_DisableClock(value);
    } else if(bus < FuriHalBusAPB1_GRP2) {
        LL_APB1_GRP1_ForceReset(value);
        LL_APB1_GRP1_DisableClock(value);
    } else if(bus < FuriHalBusAPB2_GRP1) {
        LL_APB1_GRP2_ForceReset(value);
        LL_APB1_GRP2_DisableClock(value);
    } else if(bus < FuriHalBusAPB3_GRP1) {
        LL_APB2_GRP1_ForceReset(value);
        LL_APB2_GRP1_DisableClock(value);
    } else {
        LL_APB3_GRP1_ForceReset(value);
    }
    FURI_CRITICAL_EXIT();
}
