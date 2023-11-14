#include <furi_hal_serial.h>
#include <stdbool.h>
#include <stm32wbxx_ll_lpuart.h>
#include <stm32wbxx_ll_usart.h>
#include <stm32wbxx_ll_rcc.h>
#include <furi_hal_resources.h>
#include <furi_hal_bus.h>

#include <furi.h>

typedef struct {
    bool prev_enabled[FuriHalSerialIdMax];
    FuriHalSerialRxCallback irq_cb[FuriHalSerialIdMax];
    void* irq_ctx[FuriHalSerialIdMax];
} FuriHalSerial;

static FuriHalSerial furi_hal_serial = {0};

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

    NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
}

static void furi_hal_lpuart_init(FuriHalSerialHandle* handle, uint32_t baud) {
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

    NVIC_SetPriority(LPUART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
}

void furi_hal_serial_init(FuriHalSerialHandle* handle, uint32_t baud) {
    if(handle->id == FuriHalSerialIdLpuart) {
        furi_hal_lpuart_init(handle, baud);
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
        furi_hal_gpio_init(&gpio_usart_tx, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_usart_rx, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    } else if(handle->id == FuriHalSerialIdLpuart) {
        if(furi_hal_bus_is_enabled(FuriHalBusLPUART1)) {
            furi_hal_bus_disable(FuriHalBusLPUART1);
        }
        furi_hal_gpio_init(&gpio_ext_pc0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_ext_pc1, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }
}

void furi_hal_serial_suspend(FuriHalSerialHandle* handle) {
    if(handle->id == FuriHalSerialIdLpuart && LL_LPUART_IsEnabled(LPUART1)) {
        LL_LPUART_Disable(LPUART1);
        furi_hal_serial.prev_enabled[handle->id] = true;
    } else if(handle->id == FuriHalSerialIdUsart && LL_USART_IsEnabled(USART1)) {
        LL_USART_Disable(USART1);
        furi_hal_serial.prev_enabled[handle->id] = true;
    }
}

void furi_hal_serial_resume(FuriHalSerialHandle* handle) {
    if(!furi_hal_serial.prev_enabled[handle->id]) {
        return;
    } else if(handle->id == FuriHalSerialIdLpuart) {
        LL_LPUART_Enable(LPUART1);
    } else if(handle->id == FuriHalSerialIdUsart) {
        LL_USART_Enable(USART1);
    }

    furi_hal_serial.prev_enabled[handle->id] = false;
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
    void* ctx) {
    if(callback == NULL) {
        if(handle->id == FuriHalSerialIdUsart) {
            NVIC_DisableIRQ(USART1_IRQn);
            LL_USART_DisableIT_RXNE_RXFNE(USART1);
        } else if(handle->id == FuriHalSerialIdLpuart) {
            NVIC_DisableIRQ(LPUART1_IRQn);
            LL_LPUART_DisableIT_RXNE_RXFNE(LPUART1);
        }
        furi_hal_serial.irq_cb[handle->id] = callback;
        furi_hal_serial.irq_ctx[handle->id] = ctx;
    } else {
        furi_hal_serial.irq_ctx[handle->id] = ctx;
        furi_hal_serial.irq_cb[handle->id] = callback;
        if(handle->id == FuriHalSerialIdUsart) {
            NVIC_EnableIRQ(USART1_IRQn);
            LL_USART_EnableIT_RXNE_RXFNE(USART1);
        } else if(handle->id == FuriHalSerialIdLpuart) {
            NVIC_EnableIRQ(LPUART1_IRQn);
            LL_LPUART_EnableIT_RXNE_RXFNE(LPUART1);
        }
    }
}

void LPUART1_IRQHandler(void) {
    if(LL_LPUART_IsActiveFlag_RXNE_RXFNE(LPUART1)) {
        uint8_t data = LL_LPUART_ReceiveData8(LPUART1);
        furi_hal_serial.irq_cb[FuriHalSerialIdLpuart](
            data, furi_hal_serial.irq_ctx[FuriHalSerialIdLpuart]);
    } else if(LL_LPUART_IsActiveFlag_ORE(LPUART1)) {
        LL_LPUART_ClearFlag_ORE(LPUART1);
    }
}

void USART1_IRQHandler(void) {
    if(LL_USART_IsActiveFlag_RXNE_RXFNE(USART1)) {
        uint8_t data = LL_USART_ReceiveData8(USART1);
        furi_hal_serial.irq_cb[FuriHalSerialIdUsart](
            data, furi_hal_serial.irq_ctx[FuriHalSerialIdUsart]);
    } else if(LL_USART_IsActiveFlag_ORE(USART1)) {
        LL_USART_ClearFlag_ORE(USART1);
    }
}
