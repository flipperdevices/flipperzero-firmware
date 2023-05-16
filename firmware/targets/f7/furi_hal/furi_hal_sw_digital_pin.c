#include <furi_hal_sw_digital_pin.h>

#include <furi_hal.h>
#include <stdbool.h>
#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_gpio.h>
#include <stm32wbxx_ll_dma.h>
#include <furi_hal_interrupt.h>

#include <furi.h>

/* DMA Channels definition */
#define SW_DIGITAL_PIN_TIM TIM2
#define SW_DIGITAL_PIN_TIM_IRQ FuriHalInterruptIdTIM2
#define SW_DIGITAL_PIN_DMA DMA2

#define SW_DIGITAL_PIN_TIM_CHANNEL_TX LL_TIM_CHANNEL_CH1
#define SW_DIGITAL_PIN_DMA_CH1_CHANNEL LL_DMA_CHANNEL_1
#define SW_DIGITAL_PIN_DMA_CH1_IRQ FuriHalInterruptIdDma2Ch1
#define SW_DIGITAL_PIN_DMA_CH1_DEF SW_DIGITAL_PIN_DMA, SW_DIGITAL_PIN_DMA_CH1_CHANNEL

#define SW_DIGITAL_PIN_DMA_CH2_CHANNEL LL_DMA_CHANNEL_2
#define SW_DIGITAL_PIN_TIM_CHANNEL_RX LL_TIM_CHANNEL_CH2
#define SW_DIGITAL_PIN_DMA_CH2_IRQ FuriHalInterruptIdDma2Ch2
#define SW_DIGITAL_PIN_DMA_CH2_DEF SW_DIGITAL_PIN_DMA, SW_DIGITAL_PIN_DMA_CH2_CHANNEL

#define SW_DIGITAL_PIN_TIM_CHANNEL_SYNC LL_TIM_CHANNEL_CH3
#define SW_DIGITAL_PIN_DMA_CH3_CHANNEL LL_DMA_CHANNEL_3
#define SW_DIGITAL_PIN_DMA_CH3_IRQ FuriHalInterruptIdDma2Ch3
#define SW_DIGITAL_PIN_DMA_CH3_DEF SW_DIGITAL_PIN_DMA, SW_DIGITAL_PIN_DMA_CH3_CHANNEL

//#define SW_DIGITAL_PIN_TIM_CHANNEL_SYNC_1 LL_TIM_CHANNEL_CH4
#define SW_DIGITAL_PIN_DMA_CH4_CHANNEL LL_DMA_CHANNEL_4
#define SW_DIGITAL_PIN_DMA_CH4_IRQ FuriHalInterruptIdDma2Ch4
#define SW_DIGITAL_PIN_DMA_CH4_DEF SW_DIGITAL_PIN_DMA, SW_DIGITAL_PIN_DMA_CH4_CHANNEL

#define GPIO_PIN_MAP(pin, prefix)               \
    (((pin) == (LL_GPIO_PIN_0))  ? prefix##0 :  \
     ((pin) == (LL_GPIO_PIN_1))  ? prefix##1 :  \
     ((pin) == (LL_GPIO_PIN_2))  ? prefix##2 :  \
     ((pin) == (LL_GPIO_PIN_3))  ? prefix##3 :  \
     ((pin) == (LL_GPIO_PIN_4))  ? prefix##4 :  \
     ((pin) == (LL_GPIO_PIN_5))  ? prefix##5 :  \
     ((pin) == (LL_GPIO_PIN_6))  ? prefix##6 :  \
     ((pin) == (LL_GPIO_PIN_7))  ? prefix##7 :  \
     ((pin) == (LL_GPIO_PIN_8))  ? prefix##8 :  \
     ((pin) == (LL_GPIO_PIN_9))  ? prefix##9 :  \
     ((pin) == (LL_GPIO_PIN_10)) ? prefix##10 : \
     ((pin) == (LL_GPIO_PIN_11)) ? prefix##11 : \
     ((pin) == (LL_GPIO_PIN_12)) ? prefix##12 : \
     ((pin) == (LL_GPIO_PIN_13)) ? prefix##13 : \
     ((pin) == (LL_GPIO_PIN_14)) ? prefix##14 : \
                                   prefix##15)

#define GET_DMAMUX_EXTI_LINE(pin) GPIO_PIN_MAP(pin, LL_DMAMUX_REQ_GEN_EXTI_LINE)

typedef struct {
    uint32_t sync_first_level;
    uint32_t sync_second_level;

    uint32_t* buffer_tx_ptr;
    size_t buffer_tx_size;
    size_t buffer_tx_half_size;
    size_t buffer_tx_index_write_end;

    uint16_t* buffer_rx_ptr;
    size_t buffer_rx_size;
    const GpioPin* rx_gpio;
    size_t buffer_rx_half_size;
    uint16_t tim_arr_reset;

    FuriHalSwDigitalPinTxCallbackYield tx_callback_yield;
    FuriHalSwDigitalPinTxCallbackEnd tx_callback_end;
    void* tx_context;

    FuriHalSwDigitalPinRxCallback rx_callback;
    void* rx_context;

} FuriHalSwDigitalPinBuff;

typedef struct {
    volatile SwDigitalPinState state;
    // volatile SubGhzRegulation regulation;
    // volatile FuriHalSubGhzPreset preset;
    // const GpioPin* async_mirror_pin;
} FuriHalSwDigitalPin;

volatile FuriHalSwDigitalPin furi_hal_sw_digital_pin = {
    .state = SwDigitalPinStateInit,
    //.regulation = SubGhzRegulationTxRx,
    // .preset = FuriHalSubGhzPresetIDLE,
    //.async_mirror_pin = NULL,
};

static FuriHalSwDigitalPinBuff furi_hal_sw_digital_pin_buff = {0};

// typedef enum {
//     SwDigitalPinStateIdle, /** Furi Hal SwDigitalPin is ready to start RX or TX */
//     SwDigitalPinStateAsyncRx, /** Async RX started */
//     SwDigitalPinStateAsyncTx, /** Async TX started, DMA and timer is on */
//     SwDigitalPinStateAsyncTxStopReq, /** Async TX started, async stop request received */
//     SwDigitalPinStateAsyncTxStopInProgress, /** Async TX started, stop request is processed and we wait for last data to be sent */
//     SwDigitalPinStateAsyncTxStopped, /** Async TX complete, cleanup needed */
//     SwDigitalPinStateMAX,
// } SwDigitalPinState;

static void
    furi_hal_sw_digital_pin_buff_tx_refill(uint32_t* buffer, size_t samples, size_t base_index) {
    furi_assert(furi_hal_sw_digital_pin.state == SwDigitalPinStateTx);

    while(samples > 0) {
        volatile uint32_t data = furi_hal_sw_digital_pin_buff.tx_callback_yield(
            furi_hal_sw_digital_pin_buff.tx_context);

        if(data == 0) {
            *buffer = 0;
            furi_hal_sw_digital_pin_buff.buffer_tx_index_write_end = base_index + samples;
            buffer++;
            samples--;
            LL_DMA_DisableIT_HT(SW_DIGITAL_PIN_DMA_CH1_DEF);
            LL_DMA_DisableIT_TC(SW_DIGITAL_PIN_DMA_CH1_DEF);
            LL_TIM_EnableIT_UPDATE(SW_DIGITAL_PIN_TIM);
            break;
        }

        *buffer = data;
        buffer++;
        samples--;
    }
}

static void furi_hal_sw_digital_pin_tx_timer_isr() {
    if(LL_TIM_IsActiveFlag_UPDATE(SW_DIGITAL_PIN_TIM)) {
        LL_TIM_ClearFlag_UPDATE(SW_DIGITAL_PIN_TIM);
        if(furi_hal_sw_digital_pin_buff.buffer_tx_index_write_end ==
           LL_DMA_GetDataLength(SW_DIGITAL_PIN_DMA_CH1_DEF)) {
            LL_DMA_DisableChannel(SW_DIGITAL_PIN_DMA_CH1_DEF);
            LL_TIM_DisableIT_UPDATE(SW_DIGITAL_PIN_TIM);
            furi_hal_sw_digital_pin.state = SwDigitalPinStateTxEnd;
            if(furi_hal_sw_digital_pin_buff.tx_callback_end) {
                furi_hal_sw_digital_pin_buff.tx_callback_end(
                    furi_hal_sw_digital_pin_buff.tx_context);
            }
        }
    }
}

static void furi_hal_sw_digital_pin_dma_tx_isr() {
    furi_assert(furi_hal_sw_digital_pin.state == SwDigitalPinStateTx);

#if SW_DIGITAL_PIN_DMA_CH1_CHANNEL == LL_DMA_CHANNEL_1
    if(LL_DMA_IsActiveFlag_HT1(SW_DIGITAL_PIN_DMA)) {
        LL_DMA_ClearFlag_HT1(SW_DIGITAL_PIN_DMA);

        //furi_hal_gpio_write(&gpio_ext_pa6, true);
        furi_hal_sw_digital_pin_buff_tx_refill(
            furi_hal_sw_digital_pin_buff.buffer_tx_ptr,
            furi_hal_sw_digital_pin_buff.buffer_tx_half_size,
            furi_hal_sw_digital_pin_buff.buffer_tx_half_size);
    }
    if(LL_DMA_IsActiveFlag_TC1(SW_DIGITAL_PIN_DMA)) {
        LL_DMA_ClearFlag_TC1(SW_DIGITAL_PIN_DMA);

        // furi_hal_gpio_write(&gpio_ext_pa6, false);
        furi_hal_sw_digital_pin_buff_tx_refill(
            furi_hal_sw_digital_pin_buff.buffer_tx_ptr +
                furi_hal_sw_digital_pin_buff.buffer_tx_half_size,
            furi_hal_sw_digital_pin_buff.buffer_tx_half_size,
            0);
    }
#else
#error Update this code. Would you kindly?
#endif
}

static void furi_hal_sw_digital_pin_dma_rx_isr() {
    //furi_assert(furi_hal_sw_digital_pin.state == SwDigitalPinStateTx);

#if SW_DIGITAL_PIN_DMA_CH2_CHANNEL == LL_DMA_CHANNEL_2
    if(LL_DMA_IsActiveFlag_HT2(SW_DIGITAL_PIN_DMA)) {
        LL_DMA_ClearFlag_HT2(SW_DIGITAL_PIN_DMA);

        furi_hal_gpio_write(&gpio_ext_pa6, true);

        if(furi_hal_sw_digital_pin_buff.rx_callback) {
            SwDigitalPinRx data = {
                .rx_buff = furi_hal_sw_digital_pin_buff.buffer_rx_ptr,
                .rx_buff_size = furi_hal_sw_digital_pin_buff.buffer_rx_half_size};
            furi_hal_sw_digital_pin_buff.rx_callback(
                furi_hal_sw_digital_pin_buff.rx_context, data);
        }
    }
    if(LL_DMA_IsActiveFlag_TC2(SW_DIGITAL_PIN_DMA)) {
        LL_DMA_ClearFlag_TC2(SW_DIGITAL_PIN_DMA);

        furi_hal_gpio_write(&gpio_ext_pa6, false);
        if(furi_hal_sw_digital_pin_buff.rx_callback) {
            SwDigitalPinRx data = {
                .rx_buff = furi_hal_sw_digital_pin_buff.buffer_rx_ptr +
                           furi_hal_sw_digital_pin_buff.buffer_rx_half_size,
                .rx_buff_size = furi_hal_sw_digital_pin_buff.buffer_rx_half_size};
            furi_hal_sw_digital_pin_buff.rx_callback(
                furi_hal_sw_digital_pin_buff.rx_context, data);
        }
    }
#else
#error Update this code. Would you kindly?
#endif
}

void furi_hal_sw_digital_pin_tim_init(uint16_t psc, uint32_t arr) {
    furi_assert(furi_hal_sw_digital_pin.state == SwDigitalPinStateInit);

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = psc;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = arr;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(SW_DIGITAL_PIN_TIM, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(SW_DIGITAL_PIN_TIM);
    LL_TIM_SetClockSource(SW_DIGITAL_PIN_TIM, LL_TIM_CLOCKSOURCE_INTERNAL);

    // Configure TIM channel TX
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = arr / 2;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(SW_DIGITAL_PIN_TIM, SW_DIGITAL_PIN_TIM_CHANNEL_TX, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(SW_DIGITAL_PIN_TIM, SW_DIGITAL_PIN_TIM_CHANNEL_TX);

    // // Configure TIM channel RX
    // TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
    // TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    // TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    // TIM_OC_InitStruct.CompareValue = arr / 2;
    // TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    // LL_TIM_OC_Init(SW_DIGITAL_PIN_TIM, SW_DIGITAL_PIN_TIM_CHANNEL_RX, &TIM_OC_InitStruct);
    // LL_TIM_OC_DisableFast(SW_DIGITAL_PIN_TIM, SW_DIGITAL_PIN_TIM_CHANNEL_RX);

    // Configure TIM channel SYNC
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = arr / 2;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(SW_DIGITAL_PIN_TIM, SW_DIGITAL_PIN_TIM_CHANNEL_SYNC, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(SW_DIGITAL_PIN_TIM, SW_DIGITAL_PIN_TIM_CHANNEL_SYNC);

    LL_TIM_SetTriggerOutput(SW_DIGITAL_PIN_TIM, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(SW_DIGITAL_PIN_TIM);

    // Configure Interrupt Timer
    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_TIM_IRQ, furi_hal_sw_digital_pin_tx_timer_isr, NULL);

    // Start
    LL_TIM_EnableDMAReq_UPDATE(SW_DIGITAL_PIN_TIM);
    LL_TIM_GenerateEvent_UPDATE(SW_DIGITAL_PIN_TIM);

    LL_TIM_SetCounter(SW_DIGITAL_PIN_TIM, 0);
    LL_TIM_EnableCounter(SW_DIGITAL_PIN_TIM);

    furi_hal_sw_digital_pin.state = SwDigitalPinStateIdle;
}

void furi_hal_sw_digital_pin_tim_deinit(void) {
    //furi_assert(furi_hal_sw_digital_pin.state == SwDigitalPinStateIdle);
    FURI_CRITICAL_ENTER();
    LL_TIM_DisableCounter(SW_DIGITAL_PIN_TIM);
    LL_TIM_DisableDMAReq_UPDATE(SW_DIGITAL_PIN_TIM);
    LL_TIM_DeInit(SW_DIGITAL_PIN_TIM);
    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_TIM_IRQ, NULL, NULL);
    FURI_CRITICAL_EXIT();
    //furi_hal_sw_digital_pin.state = SwDigitalPinStateInit;
}

void furi_hal_sw_digital_pin_set_tim_clock(uint16_t psc, uint32_t arr) {
    FURI_CRITICAL_ENTER();
    LL_TIM_SetPrescaler(SW_DIGITAL_PIN_TIM, psc);
    LL_TIM_SetAutoReload(SW_DIGITAL_PIN_TIM, arr);
    LL_TIM_GenerateEvent_UPDATE(SW_DIGITAL_PIN_TIM);
    FURI_CRITICAL_EXIT();
}

void furi_hal_sw_digital_pin_sync_init(const GpioPin* gpio) {
    furi_assert(gpio);
    // furi_hal_gpio_init(&gpio_ext_pb2, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    // const GpioPin* gpio = &gpio_ext_pb2;

    furi_hal_sw_digital_pin_buff.sync_first_level = (uint32_t)gpio->pin << GPIO_NUMBER;
    furi_hal_sw_digital_pin_buff.sync_second_level = gpio->pin;

    // Configure DMA
    LL_DMA_InitTypeDef dma_config = {0};
    dma_config.MemoryOrM2MDstAddress = (uint32_t)&furi_hal_sw_digital_pin_buff.sync_first_level;
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (gpio->port->BSRR);
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.NbData = 1;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM2_CH1;
    dma_config.Priority = LL_DMA_PRIORITY_MEDIUM;
    LL_DMA_Init(SW_DIGITAL_PIN_DMA_CH4_DEF, &dma_config);

    dma_config.MemoryOrM2MDstAddress = (uint32_t)&furi_hal_sw_digital_pin_buff.sync_second_level;
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (gpio->port->BSRR);
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.NbData = 1;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM2_CH3;
    dma_config.Priority = LL_DMA_PRIORITY_MEDIUM;
    LL_DMA_Init(SW_DIGITAL_PIN_DMA_CH3_DEF, &dma_config);

    // // Configure DMA Channel CC3
    LL_TIM_EnableDMAReq_CC3(SW_DIGITAL_PIN_TIM);
    LL_TIM_CC_EnableChannel(SW_DIGITAL_PIN_TIM, SW_DIGITAL_PIN_TIM_CHANNEL_SYNC);
}

void furi_hal_sw_digital_pin_sync_start(void) {
    FURI_CRITICAL_ENTER();
    uint32_t ccr3 =
        LL_TIM_GetAutoReload(SW_DIGITAL_PIN_TIM) / 2 + LL_TIM_GetCounter(SW_DIGITAL_PIN_TIM);
    if(ccr3 > LL_TIM_GetAutoReload(SW_DIGITAL_PIN_TIM)) {
        ccr3 -= LL_TIM_GetAutoReload(SW_DIGITAL_PIN_TIM);
    }
    LL_TIM_OC_SetCompareCH3(SW_DIGITAL_PIN_TIM, ccr3);

    LL_DMA_EnableChannel(SW_DIGITAL_PIN_DMA_CH4_DEF);
    LL_DMA_EnableChannel(SW_DIGITAL_PIN_DMA_CH3_DEF);
    FURI_CRITICAL_EXIT();
}

void furi_hal_sw_digital_pin_sync_stop(void) {
    FURI_CRITICAL_ENTER();
    LL_DMA_DisableChannel(SW_DIGITAL_PIN_DMA_CH3_DEF);
    LL_DMA_DisableChannel(SW_DIGITAL_PIN_DMA_CH4_DEF);
    FURI_CRITICAL_EXIT();
}

void furi_hal_sw_digital_pin_sync_deinit(void) {
    // Deinitialize DMA Channel Tx
    FURI_CRITICAL_ENTER();

    // disable DMA Channel CC3
    LL_TIM_DisableDMAReq_CC3(SW_DIGITAL_PIN_TIM);
    LL_TIM_CC_DisableChannel(SW_DIGITAL_PIN_TIM, SW_DIGITAL_PIN_TIM_CHANNEL_SYNC);

    // Deinitialize DMA
    LL_DMA_DeInit(SW_DIGITAL_PIN_DMA_CH3_DEF);
    LL_DMA_DeInit(SW_DIGITAL_PIN_DMA_CH4_DEF);

    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_DMA_CH1_IRQ, NULL, NULL);
    FURI_CRITICAL_EXIT();
}

void furi_hal_sw_digital_pin_tx_init(
    FuriHalSwDigitalPinTxCallbackYield tx_callback_yield,
    FuriHalSwDigitalPinTxCallbackEnd tx_callback_end,
    void* context,
    size_t samples,
    const GpioPin* gpio) {
    furi_assert(tx_callback_yield);
    furi_assert(samples > 0);

    //furi_hal_sw_digital_pin.state = SwDigitalPinStateTx;

    furi_hal_sw_digital_pin_buff.buffer_tx_size = samples;
    furi_hal_sw_digital_pin_buff.buffer_tx_half_size =
        furi_hal_sw_digital_pin_buff.buffer_tx_size / 2;
    furi_hal_sw_digital_pin_buff.tx_callback_yield = tx_callback_yield;
    furi_hal_sw_digital_pin_buff.tx_callback_end = tx_callback_end;
    furi_hal_sw_digital_pin_buff.tx_context = context;
    furi_hal_sw_digital_pin_buff.buffer_tx_ptr =
        malloc(furi_hal_sw_digital_pin_buff.buffer_tx_size * sizeof(uint32_t));

    //furi_hal_gpio_init(&gpio_ext_pa6, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);

    // Configure DMA
    LL_DMA_InitTypeDef dma_config = {0};
    dma_config.MemoryOrM2MDstAddress = (uint32_t)furi_hal_sw_digital_pin_buff.buffer_tx_ptr;
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (gpio->port->BSRR);
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.NbData = furi_hal_sw_digital_pin_buff.buffer_tx_size;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM2_CH1;
    dma_config.Priority = LL_DMA_PRIORITY_HIGH;
    LL_DMA_Init(SW_DIGITAL_PIN_DMA_CH1_DEF, &dma_config);

    // Configure DMA Channel CC1
    LL_TIM_EnableDMAReq_CC1(SW_DIGITAL_PIN_TIM);
    LL_TIM_CC_EnableChannel(SW_DIGITAL_PIN_TIM, SW_DIGITAL_PIN_TIM_CHANNEL_TX);

    // Start DMA irq
    furi_hal_interrupt_set_isr(
        SW_DIGITAL_PIN_DMA_CH1_IRQ, furi_hal_sw_digital_pin_dma_tx_isr, NULL);
}

void furi_hal_sw_digital_pin_tx_start(void) {
    FURI_CRITICAL_ENTER();
    furi_hal_sw_digital_pin_buff_tx_refill(
        furi_hal_sw_digital_pin_buff.buffer_tx_ptr,
        furi_hal_sw_digital_pin_buff.buffer_tx_size,
        0);
    furi_hal_sw_digital_pin.state = SwDigitalPinStateTx;
    LL_DMA_SetDataLength(SW_DIGITAL_PIN_DMA_CH1_DEF, furi_hal_sw_digital_pin_buff.buffer_tx_size);
    LL_DMA_EnableIT_TC(SW_DIGITAL_PIN_DMA_CH1_DEF);
    LL_DMA_EnableIT_HT(SW_DIGITAL_PIN_DMA_CH1_DEF);
    // Sync timer Channel Tx
    LL_TIM_OC_SetCompareCH1(SW_DIGITAL_PIN_TIM, LL_TIM_GetCounter(SW_DIGITAL_PIN_TIM));
    LL_DMA_EnableChannel(SW_DIGITAL_PIN_DMA_CH1_DEF);
    FURI_CRITICAL_EXIT();
}

bool furi_hal_sw_digital_pin_is_tx_complete(void) {
    return furi_hal_sw_digital_pin.state == SwDigitalPinStateTxEnd;
}

void furi_hal_sw_digital_pin_tx_stop(void) {
    FURI_CRITICAL_ENTER();
    if(LL_DMA_IsEnabledChannel(SW_DIGITAL_PIN_DMA_CH1_DEF))
        LL_DMA_DisableChannel(SW_DIGITAL_PIN_DMA_CH1_DEF);
    if(LL_DMA_IsEnabledIT_HT(SW_DIGITAL_PIN_DMA_CH1_DEF))
        LL_DMA_DisableIT_HT(SW_DIGITAL_PIN_DMA_CH1_DEF);
    if(LL_DMA_IsEnabledIT_TC(SW_DIGITAL_PIN_DMA_CH1_DEF))
        LL_DMA_DisableIT_TC(SW_DIGITAL_PIN_DMA_CH1_DEF);
    FURI_CRITICAL_EXIT();
}

void furi_hal_sw_digital_pin_tx_deinit(void) {
    furi_assert(
        furi_hal_sw_digital_pin.state == SwDigitalPinStateTx ||
        furi_hal_sw_digital_pin.state == SwDigitalPinStateTxLast ||
        furi_hal_sw_digital_pin.state == SwDigitalPinStateTxEnd);

    furi_hal_sw_digital_pin_tx_stop();

    // Deinitialize DMA Channel Tx
    FURI_CRITICAL_ENTER();

    // disable DMA Channel CC1
    LL_TIM_DisableDMAReq_CC1(SW_DIGITAL_PIN_TIM);
    LL_TIM_CC_DisableChannel(SW_DIGITAL_PIN_TIM, SW_DIGITAL_PIN_TIM_CHANNEL_TX);

    // Deinitialize DMA
    LL_DMA_DeInit(SW_DIGITAL_PIN_DMA_CH1_DEF);

    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_DMA_CH1_IRQ, NULL, NULL);
    FURI_CRITICAL_EXIT();

    // Deinitialize GPIO
    //furi_hal_gpio_init(&gpio_ext_pa6, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    free(furi_hal_sw_digital_pin_buff.buffer_tx_ptr);

    //furi_hal_sw_digital_pin.state = SwDigitalPinStateIdle;
}

void furi_hal_sw_digital_pin_rx_init(
    FuriHalSwDigitalPinRxCallback rx_callback,
    void* context,
    size_t samples,
    const GpioPin* gpio) {
    furi_assert(samples > 0);

    furi_hal_sw_digital_pin_buff.buffer_rx_size = samples;
    furi_hal_sw_digital_pin_buff.buffer_rx_half_size =
        furi_hal_sw_digital_pin_buff.buffer_rx_size / 2;
    furi_hal_sw_digital_pin_buff.rx_callback = rx_callback;
    furi_hal_sw_digital_pin_buff.rx_context = context;
    furi_hal_sw_digital_pin_buff.rx_gpio = gpio;
    furi_hal_sw_digital_pin_buff.buffer_rx_ptr = malloc(samples * sizeof(uint16_t));

    furi_hal_sw_digital_pin_buff.tim_arr_reset = 0;

    furi_hal_gpio_init(&gpio_ext_pa6, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);

    uint16_t arr = (64000000 / 115200) - 1;
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = 0;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = arr;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM17, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(TIM17);
    LL_TIM_SetClockSource(TIM17, LL_TIM_CLOCKSOURCE_INTERNAL);

    // Configure TIM channel TX
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = arr / 2;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(TIM17, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(TIM17, LL_TIM_CHANNEL_CH1);

    LL_TIM_SetTriggerOutput(TIM17, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM17);

    // Configure Interrupt Timer
    //furi_hal_interrupt_set_isr(TIM17, furi_hal_sw_digital_pin_tx_timer_isr, NULL);

    // Start
    //LL_TIM_EnableDMAReq_UPDATE(TIM17);
    LL_TIM_GenerateEvent_UPDATE(TIM17);

    LL_DMA_InitTypeDef dma_config = {0};
    // Config DMA Sync timer
    dma_config.MemoryOrM2MDstAddress = (uint32_t)furi_hal_sw_digital_pin_buff.tim_arr_reset;
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (TIM17->CNT);
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_NOINCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
    dma_config.NbData = 1;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_GENERATOR0;
    dma_config.Priority = LL_DMA_PRIORITY_VERYHIGH;
    LL_DMA_Init(DMA2, LL_DMA_CHANNEL_5, &dma_config);

    /* generator 0 gets fed by EXTI_LINEn */
    LL_DMAMUX_SetRequestSignalID(NULL, LL_DMAMUX_REQ_GEN_0, GET_DMAMUX_EXTI_LINE(gpio->pin));
    /* trigger on rising edge of the interrupt */
    LL_DMAMUX_SetRequestGenPolarity(NULL, LL_DMAMUX_REQ_GEN_0, LL_DMAMUX_REQ_GEN_POL_FALLING);
    /* we need the EXTI to be configured as interrupt generating line, but no ISR registered */
    furi_hal_gpio_init_ex(
        gpio, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedVeryHigh, GpioAltFnUnused);

    // Configure DMA Rx pin
    dma_config.MemoryOrM2MDstAddress = (uint32_t)furi_hal_sw_digital_pin_buff.buffer_rx_ptr;
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (gpio->port->IDR);
    dma_config.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
    dma_config.NbData = furi_hal_sw_digital_pin_buff.buffer_rx_size;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM17_CH1;
    dma_config.Priority = LL_DMA_PRIORITY_VERYHIGH;
    LL_DMA_Init(DMA2, LL_DMA_CHANNEL_2, &dma_config);

    // Configure DMA Channel CC1
    LL_TIM_EnableDMAReq_CC1(TIM17);
    LL_TIM_CC_EnableChannel(TIM17, LL_TIM_CHANNEL_CH1);

    // Start DMA irq, higher priority than normal
    furi_hal_interrupt_set_isr_ex(
        SW_DIGITAL_PIN_DMA_CH2_IRQ, 4, furi_hal_sw_digital_pin_dma_rx_isr, NULL);
    LL_DMA_EnableIT_TC(DMA2, LL_DMA_CHANNEL_2);
    LL_DMA_EnableIT_HT(DMA2, LL_DMA_CHANNEL_2);
}

void furi_hal_sw_digital_pin_rx_start(void) {
    FURI_CRITICAL_ENTER();

    LL_DMA_SetDataLength(DMA2, LL_DMA_CHANNEL_2, furi_hal_sw_digital_pin_buff.buffer_rx_size);

    // Start DMA Sync timer
    LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_5);
    /* now enable request generation again */
    LL_DMAMUX_EnableRequestGen(NULL, LL_DMAMUX_REQ_GEN_0);

    furi_hal_gpio_write(&gpio_ext_pa6, true);

    // Start DMA Rx pin
    LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_2);
    // Strat timer
    LL_TIM_SetCounter(TIM17, 0);
    LL_TIM_EnableCounter(TIM17);
    FURI_CRITICAL_EXIT();
}

void furi_hal_sw_digital_pin_rx_stop(void) {
    FURI_CRITICAL_ENTER();
    // Stop timer
    if(LL_TIM_IsEnabledCounter(TIM17)) LL_TIM_DisableCounter(TIM17);

    // Stop DMA Rx pin
    if(LL_DMA_IsEnabledChannel(DMA2, LL_DMA_CHANNEL_2)) LL_DMA_DisableChannel(DMA2, LL_DMA_CHANNEL_2);
    // Stop DMA Sync timer
    if(LL_DMA_IsEnabledChannel(DMA2, LL_DMA_CHANNEL_5)) LL_DMA_DisableChannel(DMA2, LL_DMA_CHANNEL_5);
    if(LL_DMAMUX_IsEnabledRequestGen(NULL, LL_DMAMUX_REQ_GEN_0))
        LL_DMAMUX_DisableRequestGen(NULL, LL_DMAMUX_REQ_GEN_0);
    // Stop DMA irq
    if(LL_DMA_IsEnabledIT_HT(DMA2, LL_DMA_CHANNEL_2)) LL_DMA_DisableIT_HT(DMA2, LL_DMA_CHANNEL_2);
    if(LL_DMA_IsEnabledIT_TC(DMA2, LL_DMA_CHANNEL_2)) LL_DMA_DisableIT_TC(DMA2, LL_DMA_CHANNEL_2);
    FURI_CRITICAL_EXIT();
}

void furi_hal_sw_digital_pin_rx_deinit(void) {
    // furi_assert(
    //     furi_hal_sw_digital_pin.state == SwDigitalPinStateTx ||
    //     furi_hal_sw_digital_pin.state == SwDigitalPinStateTxLast ||
    //     furi_hal_sw_digital_pin.state == SwDigitalPinStateTxEnd);

    furi_hal_sw_digital_pin_rx_stop();
    // Deinitialize DMA Channel Tx
    FURI_CRITICAL_ENTER();
    // Deinit timer
    LL_TIM_DeInit(TIM17);
    LL_TIM_DisableDMAReq_UPDATE(TIM17);

    // Deinit DMA Rx pin
    LL_DMA_DeInit(DMA2, LL_DMA_CHANNEL_2);
    // Deinit DMA Sync timer
    LL_DMA_DeInit(DMA2, LL_DMA_CHANNEL_5);

    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_DMA_CH2_IRQ, NULL, NULL);
    FURI_CRITICAL_EXIT();

    // Deinitialize GPIO
    furi_hal_gpio_init(&gpio_ext_pa6, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    //furi_hal_gpio_remove_int_callback(furi_hal_sw_digital_pin_buff.rx_gpio);
    free(furi_hal_sw_digital_pin_buff.buffer_rx_ptr);

    //furi_hal_sw_digital_pin.state = SwDigitalPinStateIdle;
}