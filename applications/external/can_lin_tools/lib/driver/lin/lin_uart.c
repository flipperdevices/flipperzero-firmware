#include "lin_uart.h"
#include "lin_bus.h"

#include <stdbool.h>
#include <stm32wbxx_ll_lpuart.h>
#include <stm32wbxx_ll_usart.h>
#include <stm32wbxx_ll_rcc.h>
#include <furi_hal_resources.h>
#include <furi_hal_bus.h>
#include <furi_hal_interrupt.h>

#define TAG "LinUart"
//#define LIN_UART USART1
#define FREQ_TIM_CLOCK 64000000

static bool lin_usart_prev_enabled;

void lin_uart_timeout_init(uint32_t baudrate, void* context);
void lin_uart_timeout_deinit(void);

static void lin_uart_irq_callback(void* context) {
    if(LL_USART_IsActiveFlag_LBD(USART1) && LL_USART_IsEnabledIT_LBD(USART1)) {
        LL_USART_ClearFlag_LBD(USART1);
        lin_bus_break_callback(context);
    }

    if(LL_USART_IsActiveFlag_FE(USART1) && LL_USART_IsEnabledIT_ERROR(USART1)) {
        LL_USART_ClearFlag_FE(USART1);
        lin_bus_error_callback(context);
    }

    if(LL_USART_IsActiveFlag_RXNE_RXFNE(USART1) && LL_USART_IsEnabledIT_RXNE_RXFNE(USART1)) {
        /* Rxne interrupt flag cleared by reading from the buffer */
        lin_uart_rx_callback(LL_USART_ReceiveData8(USART1), context);
    }

    if(LL_USART_IsActiveFlag_TXE_TXFNF(USART1) && LL_USART_IsEnabledIT_TXE_TXFNF(USART1)) {
        /* Txe interrupt flag cleared by writing to the buffer */
        // while(LL_USART_IsActiveFlag_RXNE_RXFNE(USART1))
        //     ;
        // if (LL_USART_IsActiveFlag_RXNE_RXFNE(USART1))
        //     while (1){};

        lin_uart_tx_callback(context);
    }
}

void lin_uart_init(uint32_t baudrate, void* context) {
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
    USART_InitStruct.BaudRate = baudrate;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART1, &USART_InitStruct);

    LL_USART_SetTXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
    LL_USART_SetRXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
    LL_USART_DisableFIFO(USART1);
    LL_USART_SetLINBrkDetectionLen(USART1, LL_USART_LINBREAK_DETECT_10B);
    LL_USART_DisableOverrunDetect(USART1);
    LL_USART_DisableDMADeactOnRxErr(USART1);
    LL_USART_ConfigLINMode(USART1);

    // LL_USART_EnableFIFO(USART1);
    // LL_USART_ConfigAsyncMode(USART1);

    LL_USART_Enable(USART1);

    while(!LL_USART_IsActiveFlag_TEACK(USART1) || !LL_USART_IsActiveFlag_REACK(USART1))
        ;

    //LL_USART_DisableIT_ERROR(USART1); // Disable error interrupts ??????

    furi_hal_interrupt_set_isr(FuriHalInterruptIdUart1, lin_uart_irq_callback, context);
    lin_uart_timeout_init(baudrate, context);
}

void lin_uart_set_br(uint32_t baudrate) {
    if(LL_USART_IsEnabled(USART1)) {
        // Wait for transfer complete flag
        while(!LL_USART_IsActiveFlag_TC(USART1))
            ;
        LL_USART_Disable(USART1);
        uint32_t uartclk = LL_RCC_GetUSARTClockFreq(LL_RCC_USART1_CLKSOURCE);
        LL_USART_SetBaudRate(
            USART1, uartclk, LL_USART_PRESCALER_DIV1, LL_USART_OVERSAMPLING_16, baudrate);
        LL_USART_Enable(USART1);
    }
}

void lin_uart_deinit(void) {
    //furi_hal_uart_set_irq_cb(ch, NULL, NULL);
    if(furi_hal_bus_is_enabled(FuriHalBusUSART1)) {
        furi_hal_bus_disable(FuriHalBusUSART1);
    }
    furi_hal_gpio_init(&gpio_usart_tx, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(&gpio_usart_rx, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_interrupt_set_isr(FuriHalInterruptIdUart1, NULL, NULL);
    lin_uart_timeout_deinit();
}

void lin_uart_suspend(void) {
    if(LL_USART_IsEnabled(USART1)) {
        LL_USART_Disable(USART1);
        lin_usart_prev_enabled = true;
    }
}

void lin_uart_resume(void) {
    if(!lin_usart_prev_enabled) {
        return;
    } else {
        LL_USART_Enable(USART1);
    }
    lin_usart_prev_enabled = false;
}

uint8_t lin_uart_get_char(void) {
    return LL_USART_ReceiveData8(USART1);
}

void lin_uart_put_char(uint8_t data) {
    // //Todo: blocking mode
    // while(!LL_USART_IsActiveFlag_TXE(USART1))
    //     ;
    LL_USART_TransmitData8(USART1, data);
}

void lin_uart_reset(void) {
    LL_USART_Disable(USART1);
    LL_USART_Enable(USART1);
}

void lin_uart_tx_irq_enable(void) {
    LL_USART_EnableIT_TXE(USART1);
}

void lin_uart_tx_irq_disable(void) {
    LL_USART_DisableIT_TXE(USART1);
    // todo
    LL_USART_RequestRxDataFlush(USART1);
}

void lin_uart_rx_irq_enable(void) {
    LL_USART_EnableIT_RXNE_RXFNE(USART1);
}

void lin_uart_rx_irq_disable(void) {
    LL_USART_DisableIT_RXNE_RXFNE(USART1);
}

void lin_uart_break_irq_enable(void) {
    LL_USART_EnableIT_LBD(USART1);
}

void lin_uart_break_irq_disable(void) {
    LL_USART_DisableIT_LBD(USART1);
}

void lin_uart_put_break(void) {
    LL_USART_RequestBreakSending(USART1);
}

static void lin_uart_timeout_callback(void* context) {
    if(LL_TIM_IsActiveFlag_UPDATE(TIM17)) {
        LL_TIM_ClearFlag_UPDATE(TIM17);
        lin_bus_timeout_callback(context);
    }
}

void lin_uart_timeout_init(uint32_t baudrate, void* context) {
    furi_hal_bus_enable(FuriHalBusTIM17);
    // Configure TIM
    LL_TIM_SetPrescaler(TIM17, 0);
    LL_TIM_SetCounterMode(TIM17, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(TIM17, FREQ_TIM_CLOCK - baudrate - 1);
    LL_TIM_SetClockDivision(TIM17, LL_TIM_CLOCKDIVISION_DIV1);

    // Timer: advanced
    LL_TIM_SetClockSource(TIM17, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_DisableARRPreload(TIM17);
    LL_TIM_DisableDMAReq_TRIG(TIM17);
    LL_TIM_DisableIT_TRIG(TIM17);

    furi_hal_interrupt_set_isr(
        FuriHalInterruptIdTim1TrgComTim17, lin_uart_timeout_callback, context);
}

void lin_uart_timeout_deinit(void) {
    if(LL_TIM_IsEnabledIT_UPDATE(TIM17)) {
        LL_TIM_DisableIT_UPDATE(TIM17);
    }

    furi_hal_interrupt_set_isr(FuriHalInterruptIdTim1TrgComTim17, NULL, NULL);
    furi_hal_bus_disable(FuriHalBusTIM17);
}

void lin_uart_timeout_start(void) {
    /* Reset the timer counter value */
    LL_TIM_SetCounter(TIM17, 0);
    /* Enable IRQ */
    LL_TIM_EnableIT_UPDATE(TIM17);
    /* Enable the timer */
    LL_TIM_EnableCounter(TIM17);
}

void lin_uart_timeout_stop(void) {
    /* Disable IRQ */
    LL_TIM_DisableIT_UPDATE(TIM17);
    /* Disable the timer */
    LL_TIM_DisableCounter(TIM17);
}