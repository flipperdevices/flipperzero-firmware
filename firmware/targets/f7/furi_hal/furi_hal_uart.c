#include <furi_hal_uart.h>
#include <stdbool.h>
#include <stm32wbxx_ll_lpuart.h>
#include <stm32wbxx_ll_usart.h>
#include <stm32wbxx_ll_rcc.h>
#include <stm32wbxx_ll_dma.h>
#include <furi_hal_resources.h>
#include <furi_hal_bus.h>
#include <furi_hal_interrupt.h>

#include <furi.h>

#define FURI_HAL_UART_DMA_RX_BUFFER_SIZE 256

typedef struct {
    uint8_t* buffer_rx_ptr;
    size_t buffer_rx_index_write;
    size_t buffer_rx_index_read;
    bool enabled;
    FuriHalUartRxByteCallback rx_byte_callback;
    FuriHalUartRxDMACallback rx_dma_callback;
    void* rx_callback_context;
} FuriHalUart;

static FuriHalUart* uart[FuriHalUartIdMAX];

inline void furi_hal_uart_wait_tx_complete(FuriHalUartId ch) {
    if(ch == FuriHalUartIdUSART1) {
        while(!LL_USART_IsActiveFlag_TC(USART1))
            ;
    } else if(ch == FuriHalUartIdLPUART1) {
        while(!LL_LPUART_IsActiveFlag_TC(LPUART1))
            ;
    }
}

static void furi_hal_uart_irq_callback() {
    if(LL_USART_IsActiveFlag_RXNE_RXFNE(USART1)) {
        if(uart[FuriHalUartIdUSART1]->rx_byte_callback) {
            uart[FuriHalUartIdUSART1]->rx_byte_callback(
                UartIrqEventRxByte,
                LL_LPUART_ReceiveData8(USART1),
                uart[FuriHalUartIdUSART1]->rx_callback_context);
        }
    } else if(LL_USART_IsActiveFlag_ORE(USART1)) {
        LL_USART_ClearFlag_ORE(USART1);
    }
}

static void furi_hal_usart_init(uint32_t baud) {
    furi_assert(uart[FuriHalUartIdUSART1] == NULL);
    furi_hal_bus_enable(FuriHalBusUSART1);
    uart[FuriHalUartIdUSART1] = malloc(sizeof(FuriHalUart));

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

    LL_USART_DisableIT_ERROR(USART1);
    uart[FuriHalUartIdUSART1]->enabled = true;
}

static void furi_hal_lpuart_irq_callback() {
    if(LL_LPUART_IsActiveFlag_RXNE_RXFNE(LPUART1)) {
        if(uart[FuriHalUartIdLPUART1]->rx_byte_callback) {
            uart[FuriHalUartIdLPUART1]->rx_byte_callback(
                UartIrqEventRxByte,
                LL_LPUART_ReceiveData8(LPUART1),
                uart[FuriHalUartIdLPUART1]->rx_callback_context);
        }
    } else if(LL_LPUART_IsActiveFlag_ORE(LPUART1)) {
        LL_LPUART_ClearFlag_ORE(LPUART1);
    } else if(LL_LPUART_IsActiveFlag_IDLE(LPUART1)) {
        LL_LPUART_ClearFlag_IDLE(LPUART1);
        if(uart[FuriHalUartIdLPUART1]->rx_dma_callback) {
            uart[FuriHalUartIdLPUART1]->rx_dma_callback(
                UartIrqEventRxDMAEnd,
                furi_hal_uart_dma_available(FuriHalUartIdLPUART1),
                uart[FuriHalUartIdLPUART1]->rx_callback_context);
        }
    }
}

static void furi_hal_lpuart_dma_rx_isr() {
#if LL_DMA_CHANNEL_7 == LL_DMA_CHANNEL_7
    if(LL_DMA_IsActiveFlag_HT7(DMA1)) {
        LL_DMA_ClearFlag_HT7(DMA1);
        uart[FuriHalUartIdLPUART1]->buffer_rx_index_write =
            FURI_HAL_UART_DMA_RX_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_7);
        if((uart[FuriHalUartIdLPUART1]->buffer_rx_index_read >
            uart[FuriHalUartIdLPUART1]->buffer_rx_index_write) ||
           (uart[FuriHalUartIdLPUART1]->buffer_rx_index_read <
            FURI_HAL_UART_DMA_RX_BUFFER_SIZE / 4)) {
            if(uart[FuriHalUartIdLPUART1]->rx_dma_callback) {
                uart[FuriHalUartIdLPUART1]->rx_dma_callback(
                    UartIrqEventRxDMA,
                    furi_hal_uart_dma_available(FuriHalUartIdLPUART1),
                    uart[FuriHalUartIdLPUART1]->rx_callback_context);
            }
        }

    } else if(LL_DMA_IsActiveFlag_TC7(DMA1)) {
        LL_DMA_ClearFlag_TC7(DMA1);
        uart[FuriHalUartIdLPUART1]->buffer_rx_index_write =
            FURI_HAL_UART_DMA_RX_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_7);
        if(uart[FuriHalUartIdLPUART1]->buffer_rx_index_read <
           FURI_HAL_UART_DMA_RX_BUFFER_SIZE * 3 / 4) {
            if(uart[FuriHalUartIdLPUART1]->rx_dma_callback) {
                uart[FuriHalUartIdLPUART1]->rx_dma_callback(
                    UartIrqEventRxDMA,
                    furi_hal_uart_dma_available(FuriHalUartIdLPUART1),
                    uart[FuriHalUartIdLPUART1]->rx_callback_context);
            }
        }
    }
#else
#error Update this code. Would you kindly?
#endif
}

static void furi_hal_lpuart_init_dma_rx(void) {
    /* LPUART1_RX_DMA Init */
    uart[FuriHalUartIdLPUART1]->buffer_rx_ptr = malloc(FURI_HAL_UART_DMA_RX_BUFFER_SIZE);
    LL_DMA_SetMemoryAddress(
        DMA1, LL_DMA_CHANNEL_7, (uint32_t)uart[FuriHalUartIdLPUART1]->buffer_rx_ptr);
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_7, (uint32_t) & (LPUART1->RDR));

    LL_DMA_ConfigTransfer(
        DMA1,
        LL_DMA_CHANNEL_7,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_BYTE | LL_DMA_MDATAALIGN_BYTE |
            LL_DMA_PRIORITY_HIGH);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_7, FURI_HAL_UART_DMA_RX_BUFFER_SIZE);
    LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_7, LL_DMAMUX_REQ_LPUART1_RX);

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch7, furi_hal_lpuart_dma_rx_isr, NULL);

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

static void furi_hal_lpuart_deinit_dma_rx(void) {
    if(uart[FuriHalUartIdLPUART1]->buffer_rx_ptr) {
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_7);
        LL_USART_DisableDMAReq_RX(LPUART1);

        LL_USART_DisableIT_IDLE(LPUART1);
        LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_7);
        LL_DMA_DisableIT_HT(DMA1, LL_DMA_CHANNEL_7);

        LL_DMA_ClearFlag_TC7(DMA1);
        LL_DMA_ClearFlag_HT7(DMA1);

        LL_DMA_DeInit(DMA1, LL_DMA_CHANNEL_7);
        furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch7, NULL, NULL);
        free(uart[FuriHalUartIdLPUART1]->buffer_rx_ptr);
    }
}

static void furi_hal_lpuart_init(uint32_t baud) {
    furi_assert(uart[FuriHalUartIdLPUART1] == NULL);

    furi_hal_bus_enable(FuriHalBusLPUART1);
    uart[FuriHalUartIdLPUART1] = malloc(sizeof(FuriHalUart));

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
    LPUART_InitStruct.BaudRate = 115200;
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

    furi_hal_uart_set_br(FuriHalUartIdLPUART1, baud);
    LL_LPUART_DisableIT_ERROR(LPUART1);
    uart[FuriHalUartIdLPUART1]->enabled = true;
}

void furi_hal_uart_init(FuriHalUartId ch, uint32_t baud) {
    if(ch == FuriHalUartIdLPUART1) {
        furi_hal_lpuart_init(baud);
    } else if(ch == FuriHalUartIdUSART1) {
        furi_hal_usart_init(baud);
    }
}

void furi_hal_uart_set_br(FuriHalUartId ch, uint32_t baud) {
    if(ch == FuriHalUartIdUSART1) {
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
    } else if(ch == FuriHalUartIdLPUART1) {
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

void furi_hal_uart_deinit(FuriHalUartId ch) {
    furi_hal_uart_set_irq_cb(ch, NULL, NULL);
    if(ch == FuriHalUartIdUSART1) {
        if(furi_hal_bus_is_enabled(FuriHalBusUSART1)) {
            //add deinit DMA USART1_RX
            furi_hal_bus_disable(FuriHalBusUSART1);
            free(uart[FuriHalUartIdUSART1]);
            uart[FuriHalUartIdUSART1] = NULL;
        }
        if(LL_USART_IsEnabled(USART1)) {
            LL_USART_Disable(USART1);
        }
        furi_hal_gpio_init(&gpio_usart_tx, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_usart_rx, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    } else if(ch == FuriHalUartIdLPUART1) {
        if(furi_hal_bus_is_enabled(FuriHalBusLPUART1)) {
            furi_assert(uart[FuriHalUartIdLPUART1] != NULL);
            furi_hal_bus_disable(FuriHalBusLPUART1);

            furi_hal_lpuart_deinit_dma_rx();
            free(uart[FuriHalUartIdLPUART1]);
            uart[FuriHalUartIdLPUART1] = NULL;
        }
        if(LL_LPUART_IsEnabled(LPUART1)) {
            LL_LPUART_Disable(LPUART1);
        }

        furi_hal_gpio_init(&gpio_ext_pc0, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        furi_hal_gpio_init(&gpio_ext_pc1, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }
}

void furi_hal_uart_suspend(FuriHalUartId channel) {
    furi_assert(uart[channel] != NULL);
    if(channel == FuriHalUartIdLPUART1 && LL_LPUART_IsEnabled(LPUART1)) {
        LL_LPUART_Disable(LPUART1);
    } else if(channel == FuriHalUartIdUSART1 && LL_USART_IsEnabled(USART1)) {
        LL_USART_Disable(USART1);
    }
    uart[channel]->enabled = false;
}

void furi_hal_uart_resume(FuriHalUartId channel) {
    furi_assert(uart[channel] != NULL);
    if(!uart[channel]->enabled) {
        if(channel == FuriHalUartIdLPUART1) {
            LL_LPUART_Enable(LPUART1);
        } else if(channel == FuriHalUartIdUSART1) {
            LL_USART_Enable(USART1);
        }
        uart[channel]->enabled = true;
    }
}

void furi_hal_uart_tx(FuriHalUartId ch, uint8_t* buffer, size_t buffer_size) {
    if(ch == FuriHalUartIdUSART1) {
        if(LL_USART_IsEnabled(USART1) == 0) return;

        while(buffer_size > 0) {
            while(!LL_USART_IsActiveFlag_TXE(USART1))
                ;

            LL_USART_TransmitData8(USART1, *buffer);
            buffer++;
            buffer_size--;
        }

    } else if(ch == FuriHalUartIdLPUART1) {
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

size_t furi_hal_uart_dma_available(FuriHalUartId ch) {
    if(ch == FuriHalUartIdUSART1) {
        return LL_USART_IsActiveFlag_RXNE_RXFNE(USART1) ? 1 : 0;
    } else if(ch == FuriHalUartIdLPUART1) {
        uart[FuriHalUartIdLPUART1]->buffer_rx_index_write =
            FURI_HAL_UART_DMA_RX_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_7);
        if(uart[FuriHalUartIdLPUART1]->buffer_rx_index_write >=
           uart[FuriHalUartIdLPUART1]->buffer_rx_index_read) {
            return uart[FuriHalUartIdLPUART1]->buffer_rx_index_write -
                   uart[FuriHalUartIdLPUART1]->buffer_rx_index_read;
        } else {
            return FURI_HAL_UART_DMA_RX_BUFFER_SIZE -
                   uart[FuriHalUartIdLPUART1]->buffer_rx_index_read +
                   uart[FuriHalUartIdLPUART1]->buffer_rx_index_write;
        }
    }
    return 0;
}

static uint8_t furi_hal_uart_rx_dma_read_byte(FuriHalUartId ch) {
    uint8_t data = 0;
    if(ch == FuriHalUartIdUSART1) {
        data = LL_USART_ReceiveData8(USART1);
    } else if(ch == FuriHalUartIdLPUART1) {
        data = uart[FuriHalUartIdLPUART1]
                   ->buffer_rx_ptr[uart[FuriHalUartIdLPUART1]->buffer_rx_index_read];
        uart[FuriHalUartIdLPUART1]->buffer_rx_index_read++;
        if(uart[FuriHalUartIdLPUART1]->buffer_rx_index_read >= FURI_HAL_UART_DMA_RX_BUFFER_SIZE) {
            uart[FuriHalUartIdLPUART1]->buffer_rx_index_read = 0;
        }
    }
    return data;
}

size_t furi_hal_uart_rx_dma(FuriHalUartId ch, uint8_t* data, size_t len) {
    size_t i = 0;
    size_t available = furi_hal_uart_dma_available(ch);
    if(available < len) {
        len = available;
    }
    for(i = 0; i < len; i++) {
        data[i] = furi_hal_uart_rx_dma_read_byte(ch);
    }
    return i;
}

void furi_hal_uart_set_irq_cb(FuriHalUartId ch, FuriHalUartRxByteCallback callback, void* context) {
    furi_assert(uart[ch] != NULL);
    if(ch == FuriHalUartIdUSART1) {
        if(callback == NULL) {
            furi_hal_interrupt_set_isr(FuriHalInterruptIdUart1, NULL, NULL);
            // add if init DMA -> deinit DMA
            //disable IRQ RX byte
            LL_USART_DisableIT_RXNE_RXFNE(USART1);
        } else {
            //if init DMA -> deinit DMA

            furi_hal_interrupt_set_isr(FuriHalInterruptIdUart1, furi_hal_uart_irq_callback, NULL);
            LL_USART_EnableIT_RXNE_RXFNE(USART1);
        }
    } else if(ch == FuriHalUartIdLPUART1) {
        if(callback == NULL) {
            furi_hal_interrupt_set_isr(FuriHalInterruptIdLpUart1, NULL, NULL);
            //if init DMA -> deinit DMA
            furi_hal_lpuart_deinit_dma_rx();
            //disable IRQ RX byte
            LL_LPUART_DisableIT_RXNE_RXFNE(LPUART1);
        } else {
            //if init DMA -> deinit DMA
            furi_hal_lpuart_deinit_dma_rx();
            furi_hal_interrupt_set_isr(
                FuriHalInterruptIdLpUart1, furi_hal_lpuart_irq_callback, NULL);
            LL_LPUART_EnableIT_RXNE_RXFNE(LPUART1);
        }
    }
    uart[ch]->rx_byte_callback = callback;
    uart[ch]->rx_dma_callback = NULL;
    uart[ch]->rx_callback_context = context;
}

void furi_hal_uart_set_dma_callback(
    FuriHalUartId ch,
    FuriHalUartRxDMACallback callback,
    void* context) {
    furi_assert(uart[ch] != NULL);

    if(ch == FuriHalUartIdUSART1) {
        //add Init / Deinit DMA
    } else if(ch == FuriHalUartIdLPUART1) {
        if(callback == NULL) {
            furi_hal_interrupt_set_isr(FuriHalInterruptIdLpUart1, NULL, NULL);
            //deinit DMA
            furi_hal_lpuart_deinit_dma_rx();
            //disable IRQ RX byte
            LL_LPUART_DisableIT_RXNE_RXFNE(LPUART1);
        } else {
            furi_hal_lpuart_init_dma_rx();
            furi_hal_interrupt_set_isr(
                FuriHalInterruptIdLpUart1, furi_hal_lpuart_irq_callback, NULL);
        }
    }
    uart[ch]->rx_byte_callback = NULL;
    uart[ch]->rx_dma_callback = callback;
    uart[ch]->rx_callback_context = context;
}