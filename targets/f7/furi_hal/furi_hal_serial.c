#include <furi_hal_serial.h>
#include "furi_hal_serial_types_i.h"

#include <stdbool.h>
#include <stm32wbxx_ll_lpuart.h>
#include <stm32wbxx_ll_usart.h>
#include <stm32wbxx_ll_rcc.h>
#include <stm32wbxx_ll_dma.h>
#include <furi_hal_resources.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_bus.h>

#include <furi.h>

typedef struct {
    uint8_t* buffer_rx_ptr;
    size_t buffer_rx_index_write;
    size_t buffer_rx_index_read;
    bool enabled;
    FuriHalSerialHandle* handle;
    FuriHalSerialRxCallback rx_byte_callback;
    FuriHalSerialDmaRxCallback rx_dma_callback;
    void* context;
} FuriHalSerial;

static FuriHalSerial furi_hal_serial[FuriHalSerialIdMax] = {0};

static size_t furi_hal_serial_dma_bytes_available(FuriHalSerialId ch);

//###################USART######################
static void furi_hal_serial_usart_irq_callback() {
    if(LL_USART_IsActiveFlag_RXNE_RXFNE(USART1)) {
        if(furi_hal_serial[FuriHalSerialIdUsart].rx_byte_callback) {
            furi_hal_serial[FuriHalSerialIdUsart].rx_byte_callback(
                furi_hal_serial[FuriHalSerialIdUsart].handle,
                LL_USART_ReceiveData8(USART1),
                furi_hal_serial[FuriHalSerialIdUsart].context);
        }
    } else if(LL_USART_IsActiveFlag_IDLE(USART1)) {
        LL_USART_ClearFlag_IDLE(USART1);
        if(furi_hal_serial[FuriHalSerialIdUsart].rx_dma_callback) {
            furi_hal_serial[FuriHalSerialIdUsart].rx_dma_callback(
                furi_hal_serial[FuriHalSerialIdUsart].handle,
                FuriHalSerialDmaRxEventEnd,
                furi_hal_serial_dma_bytes_available(FuriHalSerialIdUsart),
                furi_hal_serial[FuriHalSerialIdUsart].context);
        }
    } else if(LL_USART_IsActiveFlag_ORE(USART1)) {
        LL_USART_ClearFlag_ORE(USART1);
    }
}

static void furi_hal_serial_usart_dma_rx_isr() {
#if LL_DMA_CHANNEL_6 == LL_DMA_CHANNEL_6
    if(LL_DMA_IsActiveFlag_HT6(DMA1)) {
        LL_DMA_ClearFlag_HT6(DMA1);
        furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_index_write =
            FURI_HAL_SERIAL_DMA_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_6);
        if((furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_index_read >
            furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_index_write) ||
           (furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_index_read <
            FURI_HAL_SERIAL_DMA_BUFFER_SIZE / 4)) {
            if(furi_hal_serial[FuriHalSerialIdUsart].rx_dma_callback) {
                furi_hal_serial[FuriHalSerialIdUsart].rx_dma_callback(
                    furi_hal_serial[FuriHalSerialIdUsart].handle,
                    FuriHalSerialDmaRxEventRx,
                    furi_hal_serial_dma_bytes_available(FuriHalSerialIdUsart),
                    furi_hal_serial[FuriHalSerialIdUsart].context);
            }
        }

    } else if(LL_DMA_IsActiveFlag_TC6(DMA1)) {
        LL_DMA_ClearFlag_TC6(DMA1);

        if(furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_index_read <
           FURI_HAL_SERIAL_DMA_BUFFER_SIZE * 3 / 4) {
            if(furi_hal_serial[FuriHalSerialIdUsart].rx_dma_callback) {
                furi_hal_serial[FuriHalSerialIdUsart].rx_dma_callback(
                    furi_hal_serial[FuriHalSerialIdUsart].handle,
                    FuriHalSerialDmaRxEventRx,
                    furi_hal_serial_dma_bytes_available(FuriHalSerialIdUsart),
                    furi_hal_serial[FuriHalSerialIdUsart].context);
            }
        }
    }
#else
#error Update this code. Would you kindly?
#endif
}

static void furi_hal_serial_usart_init_dma_rx(void) {
    /* USART1_RX_DMA Init */
    furi_check(furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_ptr != NULL);
    furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_index_write = 0;
    furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_index_read = 0;
    furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_ptr = malloc(FURI_HAL_SERIAL_DMA_BUFFER_SIZE);
    LL_DMA_SetMemoryAddress(
        DMA1, LL_DMA_CHANNEL_6, (uint32_t)furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_ptr);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_6, (uint32_t) & (USART1->RDR));

    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_CHANNEL_6,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE | LL_DMA_MDATAALIGN_BYTE |
            LL_DMA_PRIORITY_HIGH);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_6, FURI_HAL_SERIAL_DMA_BUFFER_SIZE);
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_6, LL_DMAMUX_REQ_USART1_RX);

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch6, furi_hal_serial_usart_dma_rx_isr, NULL);

#if LL_DMA_CHANNEL_6 == LL_DMA_CHANNEL_6
    if(LL_DMA_IsActiveFlag_HT6(DMA1)) LL_DMA_ClearFlag_HT6(DMA1);
    if(LL_DMA_IsActiveFlag_TC6(DMA1)) LL_DMA_ClearFlag_TC6(DMA1);
    if(LL_DMA_IsActiveFlag_TE6(DMA1)) LL_DMA_ClearFlag_TE6(DMA1);
#else
#error Update this code. Would you kindly?
#endif

    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_6);
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_6);

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_6);
    LL_USART_EnableDMAReq_RX(USART1);

    LL_USART_EnableIT_IDLE(USART1);
}

static void furi_hal_serial_usart_deinit_dma_rx(void) {
    if(furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_ptr != NULL) {
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_6);
        LL_USART_DisableDMAReq_RX(USART1);

        LL_USART_DisableIT_IDLE(USART1);
        LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_6);
        LL_DMA_DisableIT_HT(DMA1, LL_DMA_CHANNEL_6);

        LL_DMA_ClearFlag_TC7(DMA1);
        LL_DMA_ClearFlag_HT7(DMA1);

        LL_DMA_DeInit(DMA1, LL_DMA_CHANNEL_6);
        furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch6, NULL, NULL);
        free(furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_ptr);
        furi_hal_serial[FuriHalSerialIdUsart].buffer_rx_ptr = NULL;
    }
}

static void furi_hal_serial_uasrt_init(FuriHalSerialHandle* handle, uint32_t baud) {
    furi_hal_bus_enable(FuriHalBusUSART1);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);

    furi_hal_gpio_init_ex(
        &gpio_usart_tx,
        GpioModeAltFunctionPushPull,
        GpioPullUp,
        GpioSpeedVeryHigh,
        GpioAltFn7USART1);
    furi_hal_gpio_init_ex(
        &gpio_usart_rx,
        GpioModeAltFunctionPushPull,
        GpioPullUp,
        GpioSpeedVeryHigh,
        GpioAltFn7USART1);

    LL_USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
    USART_InitStruct.BaudRate = baud;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &USART_InitStruct);
    LL_USART_EnableFIFO(USART1);
    LL_USART_ConfigAsyncMode(USART1);

    LL_USART_Enable(USART1);

    while(!LL_USART_IsActiveFlag_TEACK(USART1) || !LL_USART_IsActiveFlag_REACK(USART1))
        ;

    furi_hal_serial_set_br(handle, baud);
    LL_USART_DisableIT_ERROR(USART1);
    furi_hal_serial[handle->id].enabled = true;
}
//###################END USART###################

//###################LPUART######################
static void furi_hal_serial_lpuart_irq_callback() {
    if(LL_LPUART_IsActiveFlag_RXNE_RXFNE(LPUART1)) {
        if(furi_hal_serial[FuriHalSerialIdLpuart].rx_byte_callback) {
            furi_hal_serial[FuriHalSerialIdLpuart].rx_byte_callback(
                furi_hal_serial[FuriHalSerialIdUsart].handle,
                LL_LPUART_ReceiveData8(LPUART1),
                furi_hal_serial[FuriHalSerialIdLpuart].context);
        }
    } else if(LL_LPUART_IsActiveFlag_IDLE(LPUART1)) {
        LL_LPUART_ClearFlag_IDLE(LPUART1);
        if(furi_hal_serial[FuriHalSerialIdLpuart].rx_dma_callback) {
            furi_hal_serial[FuriHalSerialIdLpuart].rx_dma_callback(
                furi_hal_serial[FuriHalSerialIdUsart].handle,
                FuriHalSerialDmaRxEventEnd,
                furi_hal_serial_dma_bytes_available(FuriHalSerialIdLpuart),
                furi_hal_serial[FuriHalSerialIdLpuart].context);
        }
    } else if(LL_LPUART_IsActiveFlag_ORE(LPUART1)) {
        LL_LPUART_ClearFlag_ORE(LPUART1);
    }
}

static void furi_hal_serial_lpuart_dma_rx_isr() {
#if LL_DMA_CHANNEL_7 == LL_DMA_CHANNEL_7
    if(LL_DMA_IsActiveFlag_HT7(DMA1)) {
        LL_DMA_ClearFlag_HT7(DMA1);
        furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_index_write =
            FURI_HAL_SERIAL_DMA_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_7);
        if((furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_index_read >
            furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_index_write) ||
           (furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_index_read <
            FURI_HAL_SERIAL_DMA_BUFFER_SIZE / 4)) {
            if(furi_hal_serial[FuriHalSerialIdLpuart].rx_dma_callback) {
                furi_hal_serial[FuriHalSerialIdLpuart].rx_dma_callback(
                    furi_hal_serial[FuriHalSerialIdUsart].handle,
                    FuriHalSerialDmaRxEventRx,
                    furi_hal_serial_dma_bytes_available(FuriHalSerialIdLpuart),
                    furi_hal_serial[FuriHalSerialIdLpuart].context);
            }
        }

    } else if(LL_DMA_IsActiveFlag_TC7(DMA1)) {
        LL_DMA_ClearFlag_TC7(DMA1);

        if(furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_index_read <
           FURI_HAL_SERIAL_DMA_BUFFER_SIZE * 3 / 4) {
            if(furi_hal_serial[FuriHalSerialIdLpuart].rx_dma_callback) {
                furi_hal_serial[FuriHalSerialIdLpuart].rx_dma_callback(
                    furi_hal_serial[FuriHalSerialIdUsart].handle,
                    FuriHalSerialDmaRxEventRx,
                    furi_hal_serial_dma_bytes_available(FuriHalSerialIdLpuart),
                    furi_hal_serial[FuriHalSerialIdLpuart].context);
            }
        }
    }
#else
#error Update this code. Would you kindly?
#endif
}

static void furi_hal_serial_lpuart_init_dma_rx(void) {
    /* LPUART1_RX_DMA Init */
    furi_check(furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_ptr != NULL);
    furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_index_write = 0;
    furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_index_read = 0;
    furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_ptr = malloc(FURI_HAL_SERIAL_DMA_BUFFER_SIZE);
    LL_DMA_SetMemoryAddress(
        DMA1, LL_DMA_CHANNEL_7, (uint32_t)furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_ptr);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_7, (uint32_t) & (LPUART1->RDR));

    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_CHANNEL_7,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE | LL_DMA_MDATAALIGN_BYTE |
            LL_DMA_PRIORITY_HIGH);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_7, FURI_HAL_SERIAL_DMA_BUFFER_SIZE);
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_7, LL_DMAMUX_REQ_LPUART1_RX);

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch7, furi_hal_serial_lpuart_dma_rx_isr, NULL);

#if LL_DMA_CHANNEL_7 == LL_DMA_CHANNEL_7
    if(LL_DMA_IsActiveFlag_HT7(DMA1)) LL_DMA_ClearFlag_HT7(DMA1);
    if(LL_DMA_IsActiveFlag_TC7(DMA1)) LL_DMA_ClearFlag_TC7(DMA1);
    if(LL_DMA_IsActiveFlag_TE7(DMA1)) LL_DMA_ClearFlag_TE7(DMA1);
#else
#error Update this code. Would you kindly?
#endif

    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_7);
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_7);

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_7);
    LL_USART_EnableDMAReq_RX(LPUART1);

    LL_USART_EnableIT_IDLE(LPUART1);
}

static void furi_hal_serial_lpuart_deinit_dma_rx(void) {
    if(furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_ptr != NULL) {
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_7);
        LL_USART_DisableDMAReq_RX(LPUART1);

        LL_USART_DisableIT_IDLE(LPUART1);
        LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_7);
        LL_DMA_DisableIT_HT(DMA1, LL_DMA_CHANNEL_7);

        LL_DMA_ClearFlag_TC7(DMA1);
        LL_DMA_ClearFlag_HT7(DMA1);

        LL_DMA_DeInit(DMA1, LL_DMA_CHANNEL_7);
        furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch7, NULL, NULL);
        free(furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_ptr);
        furi_hal_serial[FuriHalSerialIdLpuart].buffer_rx_ptr = NULL;
    }
}

static void furi_hal_serial_lpuart_init(FuriHalSerialHandle* handle, uint32_t baud) {
    furi_hal_bus_enable(FuriHalBusLPUART1);
    LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_PCLK1);

    furi_hal_gpio_init_ex(
        &gpio_ext_pc0,
        GpioModeAltFunctionPushPull,
        GpioPullUp,
        GpioSpeedVeryHigh,
        GpioAltFn8LPUART1);
    furi_hal_gpio_init_ex(
        &gpio_ext_pc1,
        GpioModeAltFunctionPushPull,
        GpioPullUp,
        GpioSpeedVeryHigh,
        GpioAltFn8LPUART1);

    LL_LPUART_InitTypeDef LPUART_InitStruct;
    LPUART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;
    LPUART_InitStruct.BaudRate = baud;
    LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
    LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
    LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
    LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
    LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
    LL_LPUART_Init(LPUART1, &LPUART_InitStruct);
    LL_LPUART_EnableFIFO(LPUART1);

    LL_LPUART_Enable(LPUART1);

    while(!LL_LPUART_IsActiveFlag_TEACK(LPUART1) || !LL_LPUART_IsActiveFlag_REACK(LPUART1))
        ;

    furi_hal_serial_set_br(handle, baud);
    LL_LPUART_DisableIT_ERROR(LPUART1);
    furi_hal_serial[handle->id].enabled = true;
}
//###################END LPUART###################

void furi_hal_serial_init(FuriHalSerialHandle* handle, uint32_t baud) {
    if(handle->id == FuriHalSerialIdLpuart) {
        furi_hal_serial_lpuart_init(handle, baud);
    } else if(handle->id == FuriHalSerialIdUsart) {
        furi_hal_serial_uasrt_init(handle, baud);
    }
}

void furi_hal_serial_set_br(FuriHalSerialHandle* handle, uint32_t baud) {
    if(handle->id == FuriHalSerialIdUsart) {
        if(LL_USART_IsEnabled(USART1)) {
            // Wait for transfer complete flag
            while(!LL_USART_IsActiveFlag_TC(USART1))
                ;
            LL_USART_Disable(USART1);
            uint32_t uartclk = LL_RCC_GetUSARTClockFreq(LL_RCC_USART1_CLKSOURCE);
            LL_USART_SetBaudRate(
                USART1, uartclk, LL_USART_PRESCALER_DIV1, LL_USART_OVERSAMPLING_16, baud);
            LL_USART_Enable(USART1);
        }
    } else if(handle->id == FuriHalSerialIdLpuart) {
        if(LL_LPUART_IsEnabled(LPUART1)) {
            // Wait for transfer complete flag
            while(!LL_LPUART_IsActiveFlag_TC(LPUART1))
                ;
            LL_LPUART_Disable(LPUART1);
            uint32_t uartclk = LL_RCC_GetLPUARTClockFreq(LL_RCC_LPUART1_CLKSOURCE);
            if(uartclk / baud > 4095) {
                LL_LPUART_SetPrescaler(LPUART1, LL_LPUART_PRESCALER_DIV32);
                LL_LPUART_SetBaudRate(LPUART1, uartclk, LL_LPUART_PRESCALER_DIV32, baud);
            } else {
                LL_LPUART_SetPrescaler(LPUART1, LL_LPUART_PRESCALER_DIV1);
                LL_LPUART_SetBaudRate(LPUART1, uartclk, LL_LPUART_PRESCALER_DIV1, baud);
            }
            LL_LPUART_Enable(LPUART1);
        }
    }
}

void furi_hal_serial_deinit(FuriHalSerialHandle* handle) {
    furi_hal_serial_set_rx_callback(handle, NULL, NULL);
    if(handle->id == FuriHalSerialIdUsart) {
        if(furi_hal_bus_is_enabled(FuriHalBusUSART1)) {
            furi_hal_bus_disable(FuriHalBusUSART1);
        }
        if(LL_USART_IsEnabled(USART1)) {
            LL_USART_Disable(USART1);
        }
        furi_hal_serial_usart_deinit_dma_rx();
        furi_hal_gpio_init(&gpio_usart_tx, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_usart_rx, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_serial[handle->id].enabled = false;
    } else if(handle->id == FuriHalSerialIdLpuart) {
        if(furi_hal_bus_is_enabled(FuriHalBusLPUART1)) {
            furi_hal_bus_disable(FuriHalBusLPUART1);
        }
        if(LL_LPUART_IsEnabled(LPUART1)) {
            LL_LPUART_Disable(LPUART1);
        }
        furi_hal_serial_lpuart_deinit_dma_rx();
        furi_hal_gpio_init(&gpio_ext_pc0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_ext_pc1, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_serial[handle->id].enabled = false;
    }
}

void furi_hal_serial_suspend(FuriHalSerialHandle* handle) {
    if(handle->id == FuriHalSerialIdLpuart && LL_LPUART_IsEnabled(LPUART1)) {
        LL_LPUART_Disable(LPUART1);
    } else if(handle->id == FuriHalSerialIdUsart && LL_USART_IsEnabled(USART1)) {
        LL_USART_Disable(USART1);
    }
    furi_hal_serial[handle->id].enabled = false;
}

void furi_hal_serial_resume(FuriHalSerialHandle* handle) {
    if(!furi_hal_serial[handle->id].enabled) {
        if(handle->id == FuriHalSerialIdLpuart) {
            LL_LPUART_Enable(LPUART1);
        } else if(handle->id == FuriHalSerialIdUsart) {
            LL_USART_Enable(USART1);
        }
        furi_hal_serial[handle->id].enabled = true;
    }
}

void furi_hal_serial_tx(FuriHalSerialHandle* handle, const uint8_t* buffer, size_t buffer_size) {
    if(handle->id == FuriHalSerialIdUsart) {
        if(LL_USART_IsEnabled(USART1) == 0) return;

        while(buffer_size > 0) {
            while(!LL_USART_IsActiveFlag_TXE(USART1))
                ;

            LL_USART_TransmitData8(USART1, *buffer);
            buffer++;
            buffer_size--;
        }

    } else if(handle->id == FuriHalSerialIdLpuart) {
        if(LL_LPUART_IsEnabled(LPUART1) == 0) return;

        while(buffer_size > 0) {
            while(!LL_LPUART_IsActiveFlag_TXE(LPUART1))
                ;

            LL_LPUART_TransmitData8(LPUART1, *buffer);

            buffer++;
            buffer_size--;
        }
    }
}

void furi_hal_serial_tx_wait_complete(FuriHalSerialHandle* handle) {
    if(handle->id == FuriHalSerialIdUsart) {
        if(LL_USART_IsEnabled(USART1) == 0) return;

        while(!LL_USART_IsActiveFlag_TC(USART1))
            ;
    } else if(handle->id == FuriHalSerialIdLpuart) {
        if(LL_LPUART_IsEnabled(LPUART1) == 0) return;

        while(!LL_LPUART_IsActiveFlag_TC(LPUART1))
            ;
    }
}

void furi_hal_serial_set_rx_callback(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxCallback callback,
    void* context) {
    if(handle->id == FuriHalSerialIdUsart) {
        if(callback == NULL) {
            furi_hal_interrupt_set_isr(FuriHalInterruptIdUart1, NULL, NULL);
            //if init DMA -> deinit DMA
            furi_hal_serial_usart_deinit_dma_rx();
            //disable IRQ RX byte
            LL_USART_DisableIT_RXNE_RXFNE(USART1);
        } else {
            //if init DMA -> deinit DMA
            furi_hal_serial_usart_deinit_dma_rx();
            furi_hal_interrupt_set_isr(
                FuriHalInterruptIdUart1, furi_hal_serial_usart_irq_callback, NULL);
            LL_USART_EnableIT_RXNE_RXFNE(USART1);
        }
    } else if(handle->id == FuriHalSerialIdLpuart) {
        if(callback == NULL) {
            furi_hal_interrupt_set_isr(FuriHalInterruptIdLpUart1, NULL, NULL);
            //if init DMA -> deinit DMA
            furi_hal_serial_lpuart_deinit_dma_rx();
            //disable IRQ RX byte
            LL_LPUART_DisableIT_RXNE_RXFNE(LPUART1);
        } else {
            //if init DMA -> deinit DMA
            furi_hal_serial_lpuart_deinit_dma_rx();
            furi_hal_interrupt_set_isr(
                FuriHalInterruptIdLpUart1, furi_hal_serial_lpuart_irq_callback, NULL);
            LL_LPUART_EnableIT_RXNE_RXFNE(LPUART1);
        }
    }
    furi_hal_serial[handle->id].rx_byte_callback = callback;
    furi_hal_serial[handle->id].handle = handle;
    furi_hal_serial[handle->id].rx_dma_callback = NULL;
    furi_hal_serial[handle->id].context = context;
}

static size_t furi_hal_serial_dma_bytes_available(FuriHalSerialId ch) {
    size_t index_dma = 0;
    if(ch == FuriHalSerialIdUsart) {
        index_dma = LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_6);
    } else if(ch == FuriHalSerialIdLpuart) {
        index_dma = LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_7);
    } else {
        return 0;
    }

    furi_hal_serial[ch].buffer_rx_index_write = FURI_HAL_SERIAL_DMA_BUFFER_SIZE - index_dma;
    if(furi_hal_serial[ch].buffer_rx_index_write >= furi_hal_serial[ch].buffer_rx_index_read) {
        return furi_hal_serial[ch].buffer_rx_index_write -
               furi_hal_serial[ch].buffer_rx_index_read;
    } else {
        return FURI_HAL_SERIAL_DMA_BUFFER_SIZE - furi_hal_serial[ch].buffer_rx_index_read +
               furi_hal_serial[ch].buffer_rx_index_write;
    }
}

static uint8_t furi_hal_serial_dma_rx_read_byte(FuriHalSerialHandle* handle) {
    uint8_t data = 0;
    data =
        furi_hal_serial[handle->id].buffer_rx_ptr[furi_hal_serial[handle->id].buffer_rx_index_read];
    furi_hal_serial[handle->id].buffer_rx_index_read++;
    if(furi_hal_serial[handle->id].buffer_rx_index_read >= FURI_HAL_SERIAL_DMA_BUFFER_SIZE) {
        furi_hal_serial[handle->id].buffer_rx_index_read = 0;
    }
    return data;
}

size_t furi_hal_serial_dma_rx(FuriHalSerialHandle* handle, uint8_t* data, size_t len) {
    furi_check(FURI_IS_IRQ_MODE());
    furi_assert(furi_hal_serial[handle->id].buffer_rx_ptr != NULL);
    size_t i = 0;
    size_t available = furi_hal_serial_dma_bytes_available(handle->id);
    if(available < len) {
        len = available;
    }
    for(i = 0; i < len; i++) {
        data[i] = furi_hal_serial_dma_rx_read_byte(handle);
    }
    return i;
}

void furi_hal_serial_dma_start(
    FuriHalSerialHandle* handle,
    FuriHalSerialDmaRxCallback callback,
    void* context) {
    if(handle->id == FuriHalSerialIdUsart) {
        if(callback == NULL) {
            furi_hal_interrupt_set_isr(FuriHalInterruptIdUart1, NULL, NULL);
            //deinit DMA
            furi_hal_serial_usart_init_dma_rx();
            //disable IRQ RX byte
            LL_USART_DisableIT_RXNE_RXFNE(USART1);
        } else {
            //deinit DMA
            furi_hal_serial_usart_init_dma_rx();
            furi_hal_interrupt_set_isr(
                FuriHalInterruptIdUart1, furi_hal_serial_usart_irq_callback, NULL);
        }
    } else if(handle->id == FuriHalSerialIdLpuart) {
        if(callback == NULL) {
            furi_hal_interrupt_set_isr(FuriHalInterruptIdLpUart1, NULL, NULL);
            //deinit DMA
            furi_hal_serial_lpuart_deinit_dma_rx();
            //disable IRQ RX byte
            LL_LPUART_DisableIT_RXNE_RXFNE(LPUART1);
        } else {
            furi_hal_serial_lpuart_init_dma_rx();
            furi_hal_interrupt_set_isr(
                FuriHalInterruptIdLpUart1, furi_hal_serial_lpuart_irq_callback, NULL);
        }
    }
    furi_hal_serial[handle->id].rx_byte_callback = NULL;
    furi_hal_serial[handle->id].handle = handle;
    furi_hal_serial[handle->id].rx_dma_callback = callback;
    furi_hal_serial[handle->id].context = context;
}