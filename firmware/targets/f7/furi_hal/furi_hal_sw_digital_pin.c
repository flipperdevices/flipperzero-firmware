#include <furi_hal_sw_digital_pin.h>

#include <furi_hal.h>
#include <stdbool.h>
#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_gpio.h>
#include <stm32wbxx_ll_dma.h>
#include <furi_hal_interrupt.h>

#include <furi.h>

#define FURI_HAL_SW_DIGITAL_DEBUG_PIN_RX
#define FURI_HAL_SW_DIGITAL_DEBUG_PIN_TX

#ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_RX
const GpioPin* gpio_debug_rx = &gpio_ext_pb3;
#endif

#ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_TX
const GpioPin* gpio_debug_tx = &gpio_ext_pa6;
#endif

/* DMA Channels definition */
#define SW_DIGITAL_PIN_DMA DMA2

// Tx
#define SW_DIGITAL_PIN_TIM_TX TIM2
#define SW_DIGITAL_PIN_TIM_IRQ FuriHalInterruptIdTIM2

#define SW_DIGITAL_PIN_TIM_CHANNEL_TX LL_TIM_CHANNEL_CH1
#define SW_DIGITAL_PIN_DMA_CHANNEL_TX LL_DMA_CHANNEL_1
#define SW_DIGITAL_PIN_DMA_IRQ_TX FuriHalInterruptIdDma2Ch1
#define SW_DIGITAL_PIN_DMA_DEF_TX SW_DIGITAL_PIN_DMA, SW_DIGITAL_PIN_DMA_CHANNEL_TX

// Rx
#define SW_DIGITAL_PIN_DMA_CHANNEL_RX LL_DMA_CHANNEL_2
#define SW_DIGITAL_PIN_DMA_IRQ_RX FuriHalInterruptIdDma2Ch2
#define SW_DIGITAL_PIN_DMA_DEF_RX SW_DIGITAL_PIN_DMA, SW_DIGITAL_PIN_DMA_CHANNEL_RX
// Rx Sync
#define SW_DIGITAL_PIN_TIM_RX TIM17
#define SW_DIGITAL_PIN_TIM_CHANNEL_SYNC_RX LL_TIM_CHANNEL_CH1

#define SW_DIGITAL_PIN_DMA_CHANNEL_SYNC_RX LL_DMA_CHANNEL_5
#define SW_DIGITAL_PIN_DMA_SYNC_RX_IRQ FuriHalInterruptIdDma2Ch5
#define SW_DIGITAL_PIN_DMA_SYNC_RX_DEF SW_DIGITAL_PIN_DMA, SW_DIGITAL_PIN_DMA_CHANNEL_SYNC_RX

// Sync
#define SW_DIGITAL_PIN_TIM_CHANNEL_SYNC_1 LL_TIM_CHANNEL_CH3
#define SW_DIGITAL_PIN_DMA_CHANNEL_SYNC_1 LL_DMA_CHANNEL_3
#define SW_DIGITAL_PIN_DMA_IRQ_SYNC_1 FuriHalInterruptIdDma2Ch3
#define SW_DIGITAL_PIN_DMA_DEF_SYNC_1 SW_DIGITAL_PIN_DMA, SW_DIGITAL_PIN_DMA_CHANNEL_SYNC_1

//#define SW_DIGITAL_PIN_TIM_CHANNEL_SYNC_1 LL_TIM_CHANNEL_CH4
#define SW_DIGITAL_PIN_DMA_CHANNEL_SYNC_2 LL_DMA_CHANNEL_4
#define SW_DIGITAL_PIN_DMA_IRQ_SYNC_2 FuriHalInterruptIdDma2Ch4
#define SW_DIGITAL_PIN_DMA_DEF_SYNC_2 SW_DIGITAL_PIN_DMA, SW_DIGITAL_PIN_DMA_CHANNEL_SYNC_2

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
    // Sync
    uint32_t sync_first_level;
    uint32_t sync_second_level;

    // Tx
    const GpioPin* tx_gpio;
    uint32_t* buffer_tx_ptr;
    size_t buffer_tx_size;
    size_t buffer_tx_half_size;
    size_t buffer_tx_index_write_end;
    FuriHalSwDigitalPinTxCallbackYield tx_callback_yield;
    FuriHalSwDigitalPinTxCallbackEnd tx_callback_end;
    void* tx_context;

    // Rx
    const GpioPin* rx_gpio;
    uint16_t* buffer_rx_ptr;
    size_t buffer_rx_size;
    size_t buffer_rx_half_size;
    uint16_t tim_arr_reset;
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

static void furi_hal_sw_digital_pin_buff_tx_refill(
    uint32_t* buffer,
    volatile size_t samples,
    size_t base_index) {
    furi_assert(furi_hal_sw_digital_pin.state == SwDigitalPinStateTx);

    do {
        uint32_t data = furi_hal_sw_digital_pin_buff.tx_callback_yield(
            furi_hal_sw_digital_pin_buff.tx_context);

        if(data == 0) {
            *buffer = 0;
            furi_hal_sw_digital_pin_buff.buffer_tx_index_write_end = base_index + samples;

            buffer++;
            samples--;
            LL_DMA_DisableIT_HT(SW_DIGITAL_PIN_DMA_DEF_TX);
            LL_DMA_DisableIT_TC(SW_DIGITAL_PIN_DMA_DEF_TX);
            LL_TIM_EnableIT_UPDATE(SW_DIGITAL_PIN_TIM_TX);
            break;
        }

        *buffer = data;
        buffer++;
        samples--;
    } while(samples > 0);
}

static void furi_hal_sw_digital_pin_tx_timer_isr() {
    if(LL_TIM_IsActiveFlag_UPDATE(SW_DIGITAL_PIN_TIM_TX)) {
        LL_TIM_ClearFlag_UPDATE(SW_DIGITAL_PIN_TIM_TX);
#ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_TX
        furi_hal_gpio_write(gpio_debug_tx, false);
#endif
        if(furi_hal_sw_digital_pin_buff.buffer_tx_index_write_end ==
           LL_DMA_GetDataLength(SW_DIGITAL_PIN_DMA_DEF_TX)) {
            LL_DMA_DisableChannel(SW_DIGITAL_PIN_DMA_DEF_TX);
            LL_TIM_DisableIT_UPDATE(SW_DIGITAL_PIN_TIM_TX);
            furi_hal_sw_digital_pin.state = SwDigitalPinStateTxEnd;
            if(furi_hal_sw_digital_pin_buff.tx_callback_end) {
                furi_hal_sw_digital_pin_buff.tx_callback_end(
                    furi_hal_sw_digital_pin_buff.tx_context);
            }
        }
#ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_TX
        furi_hal_gpio_write(gpio_debug_tx, true);
#endif
    }
}

static void furi_hal_sw_digital_pin_dma_tx_isr() {
    furi_assert(furi_hal_sw_digital_pin.state == SwDigitalPinStateTx);

#if SW_DIGITAL_PIN_DMA_CHANNEL_TX == LL_DMA_CHANNEL_1
    if(LL_DMA_IsActiveFlag_HT1(SW_DIGITAL_PIN_DMA)) {
        LL_DMA_ClearFlag_HT1(SW_DIGITAL_PIN_DMA);
        // #ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_TX
        //         furi_hal_gpio_write(gpio_debug_tx, false);
        // #endif
        furi_hal_gpio_write(gpio_debug_tx, false);
        furi_hal_sw_digital_pin_buff_tx_refill(
            furi_hal_sw_digital_pin_buff.buffer_tx_ptr,
            furi_hal_sw_digital_pin_buff.buffer_tx_half_size,
            furi_hal_sw_digital_pin_buff.buffer_tx_half_size);
        furi_hal_gpio_write(gpio_debug_tx, true);
    }
    if(LL_DMA_IsActiveFlag_TC1(SW_DIGITAL_PIN_DMA)) {
        LL_DMA_ClearFlag_TC1(SW_DIGITAL_PIN_DMA);
        // #ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_TX
        //         furi_hal_gpio_write(gpio_debug_tx, true);
        // #endif
        furi_hal_gpio_write(gpio_debug_tx, false);
        furi_hal_sw_digital_pin_buff_tx_refill(
            furi_hal_sw_digital_pin_buff.buffer_tx_ptr +
                furi_hal_sw_digital_pin_buff.buffer_tx_half_size,
            furi_hal_sw_digital_pin_buff.buffer_tx_half_size,
            0);
        furi_hal_gpio_write(gpio_debug_tx, true);
    }
#else
#error Update this code. Would you kindly?
#endif
}

static void furi_hal_sw_digital_pin_dma_rx_isr() {
    //furi_assert(furi_hal_sw_digital_pin.state == SwDigitalPinStateTx);

#if SW_DIGITAL_PIN_DMA_CHANNEL_RX == LL_DMA_CHANNEL_2
    if(LL_DMA_IsActiveFlag_HT2(SW_DIGITAL_PIN_DMA)) {
        LL_DMA_ClearFlag_HT2(SW_DIGITAL_PIN_DMA);

        // #ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_RX
        //         furi_hal_gpio_write(gpio_debug_rx, false);
        // #endif
        furi_hal_gpio_write(gpio_debug_rx, false);
        if(furi_hal_sw_digital_pin_buff.rx_callback) {
            SwDigitalPinRx data = {
                .rx_buff = furi_hal_sw_digital_pin_buff.buffer_rx_ptr,
                .rx_buff_size = furi_hal_sw_digital_pin_buff.buffer_rx_half_size};
            furi_hal_sw_digital_pin_buff.rx_callback(
                furi_hal_sw_digital_pin_buff.rx_context, data);
        }
        furi_hal_gpio_write(gpio_debug_rx, true);
    }
    if(LL_DMA_IsActiveFlag_TC2(SW_DIGITAL_PIN_DMA)) {
        LL_DMA_ClearFlag_TC2(SW_DIGITAL_PIN_DMA);

        // #ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_RX
        //         furi_hal_gpio_write(gpio_debug_rx, true);
        // #endif
        furi_hal_gpio_write(gpio_debug_rx, false);
        if(furi_hal_sw_digital_pin_buff.rx_callback) {
            SwDigitalPinRx data = {
                .rx_buff = furi_hal_sw_digital_pin_buff.buffer_rx_ptr +
                           furi_hal_sw_digital_pin_buff.buffer_rx_half_size,
                .rx_buff_size = furi_hal_sw_digital_pin_buff.buffer_rx_half_size};
            furi_hal_sw_digital_pin_buff.rx_callback(
                furi_hal_sw_digital_pin_buff.rx_context, data);
        }
        furi_hal_gpio_write(gpio_debug_rx, true);
    }
#else
#error Update this code. Would you kindly?
#endif
}

void furi_hal_sw_digital_pin_sync_init(const GpioPin* gpio) {
    furi_assert(gpio);
    // furi_hal_gpio_init(&gpio_ext_pb2, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    // const GpioPin* gpio = &gpio_ext_pb2;

    furi_hal_sw_digital_pin_buff.sync_first_level = (uint32_t)gpio->pin << GPIO_NUMBER;
    furi_hal_sw_digital_pin_buff.sync_second_level = gpio->pin;

    // Configure DMA sync first level
    LL_DMA_SetMemoryAddress(
        SW_DIGITAL_PIN_DMA_DEF_SYNC_2, (uint32_t)&furi_hal_sw_digital_pin_buff.sync_first_level);
    LL_DMA_SetPeriphAddress(SW_DIGITAL_PIN_DMA_DEF_SYNC_2, (uint32_t) & (gpio->port->BSRR));
    LL_DMA_ConfigTransfer(
        SW_DIGITAL_PIN_DMA_DEF_SYNC_2,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_NOINCREMENT | LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD |
            LL_DMA_PRIORITY_MEDIUM);
    LL_DMA_SetDataLength(SW_DIGITAL_PIN_DMA_DEF_SYNC_2, 1);
    LL_DMA_SetPeriphRequest(SW_DIGITAL_PIN_DMA_DEF_SYNC_2, LL_DMAMUX_REQ_TIM2_CH1);

    // Configure DMA sync second level
    LL_DMA_SetMemoryAddress(
        SW_DIGITAL_PIN_DMA_DEF_SYNC_1, (uint32_t)&furi_hal_sw_digital_pin_buff.sync_second_level);
    LL_DMA_SetPeriphAddress(SW_DIGITAL_PIN_DMA_DEF_SYNC_1, (uint32_t) & (gpio->port->BSRR));
    LL_DMA_ConfigTransfer(
        SW_DIGITAL_PIN_DMA_DEF_SYNC_1,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_NOINCREMENT | LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD |
            LL_DMA_PRIORITY_MEDIUM);
    LL_DMA_SetDataLength(SW_DIGITAL_PIN_DMA_DEF_SYNC_1, 1);
    LL_DMA_SetPeriphRequest(SW_DIGITAL_PIN_DMA_DEF_SYNC_1, LL_DMAMUX_REQ_TIM2_CH3);

    // // Configure DMA Channel CC3
    LL_TIM_EnableDMAReq_CC3(SW_DIGITAL_PIN_TIM_TX);
    LL_TIM_CC_EnableChannel(SW_DIGITAL_PIN_TIM_TX, SW_DIGITAL_PIN_TIM_CHANNEL_SYNC_1);
}

void furi_hal_sw_digital_pin_sync_start(void) {
    FURI_CRITICAL_ENTER();
    uint32_t ccr3 =
        LL_TIM_GetAutoReload(SW_DIGITAL_PIN_TIM_TX) / 2 + LL_TIM_GetCounter(SW_DIGITAL_PIN_TIM_TX);
    if(ccr3 > LL_TIM_GetAutoReload(SW_DIGITAL_PIN_TIM_TX)) {
        ccr3 -= LL_TIM_GetAutoReload(SW_DIGITAL_PIN_TIM_TX);
    }
    LL_TIM_OC_SetCompareCH3(SW_DIGITAL_PIN_TIM_TX, ccr3);

    LL_DMA_EnableChannel(SW_DIGITAL_PIN_DMA_DEF_SYNC_2);
    LL_DMA_EnableChannel(SW_DIGITAL_PIN_DMA_DEF_SYNC_1);
    FURI_CRITICAL_EXIT();
}

void furi_hal_sw_digital_pin_sync_stop(void) {
    FURI_CRITICAL_ENTER();
    LL_DMA_DisableChannel(SW_DIGITAL_PIN_DMA_DEF_SYNC_1);
    LL_DMA_DisableChannel(SW_DIGITAL_PIN_DMA_DEF_SYNC_2);
    FURI_CRITICAL_EXIT();
}

void furi_hal_sw_digital_pin_sync_deinit(void) {
    // Deinitialize DMA Channel Tx
    FURI_CRITICAL_ENTER();

    // disable DMA Channel CC3
    LL_TIM_DisableDMAReq_CC3(SW_DIGITAL_PIN_TIM_TX);
    LL_TIM_CC_DisableChannel(SW_DIGITAL_PIN_TIM_TX, SW_DIGITAL_PIN_TIM_CHANNEL_SYNC_1);

    // Deinitialize DMA
    LL_DMA_DeInit(SW_DIGITAL_PIN_DMA_DEF_SYNC_1);
    LL_DMA_DeInit(SW_DIGITAL_PIN_DMA_DEF_SYNC_2);

    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_DMA_IRQ_TX, NULL, NULL);
    FURI_CRITICAL_EXIT();
}

void furi_hal_sw_digital_pin_tx_init(
    FuriHalSwDigitalPinTxCallbackYield tx_callback_yield,
    FuriHalSwDigitalPinTxCallbackEnd tx_callback_end,
    void* context,
    uint16_t tim_psc,
    uint32_t tim_arr,
    size_t samples,
    const GpioPin* gpio) {
    furi_assert(tx_callback_yield);
    furi_assert(samples > 0);
    furi_assert(!(samples & 0x1));

#ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_TX
    furi_hal_gpio_init(gpio_debug_tx, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(gpio_debug_tx, false);
#endif

    //furi_hal_sw_digital_pin.state = SwDigitalPinStateTx;
    furi_hal_sw_digital_pin_buff.tx_gpio = gpio;

    furi_hal_sw_digital_pin_buff.buffer_tx_size = samples;
    furi_hal_sw_digital_pin_buff.buffer_tx_half_size =
        furi_hal_sw_digital_pin_buff.buffer_tx_size / 2;
    furi_hal_sw_digital_pin_buff.buffer_tx_ptr =
        malloc(furi_hal_sw_digital_pin_buff.buffer_tx_size * sizeof(uint32_t));

    furi_hal_sw_digital_pin_buff.tx_callback_yield = tx_callback_yield;
    furi_hal_sw_digital_pin_buff.tx_callback_end = tx_callback_end;
    furi_hal_sw_digital_pin_buff.tx_context = context;

    furi_hal_gpio_init(gpio, GpioModeOutputPushPull, GpioPullNo, GpioSpeedHigh);

    //furi_hal_gpio_init(gpio_debug_rx, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);

    // Configure TIM
    LL_TIM_SetPrescaler(SW_DIGITAL_PIN_TIM_TX, tim_psc);
    LL_TIM_SetCounterMode(SW_DIGITAL_PIN_TIM_TX, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(SW_DIGITAL_PIN_TIM_TX, tim_arr);
    LL_TIM_SetClockDivision(SW_DIGITAL_PIN_TIM_TX, LL_TIM_CLOCKDIVISION_DIV1);

    LL_TIM_DisableARRPreload(SW_DIGITAL_PIN_TIM_TX);
    LL_TIM_SetClockSource(SW_DIGITAL_PIN_TIM_TX, LL_TIM_CLOCKSOURCE_INTERNAL);

    // Configure TIM channel TX
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = tim_arr / 2;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(SW_DIGITAL_PIN_TIM_TX, SW_DIGITAL_PIN_TIM_CHANNEL_TX, &TIM_OC_InitStruct);
    LL_TIM_OC_DisableFast(SW_DIGITAL_PIN_TIM_TX, SW_DIGITAL_PIN_TIM_CHANNEL_TX);

    // // Configure TIM channel SYNC
    // TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
    // TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    // TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    // TIM_OC_InitStruct.CompareValue = tim_arr / 2;
    // TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    // LL_TIM_OC_Init(SW_DIGITAL_PIN_TIM_TX, SW_DIGITAL_PIN_TIM_CHANNEL_SYNC_1, &TIM_OC_InitStruct);
    // LL_TIM_OC_DisableFast(SW_DIGITAL_PIN_TIM_TX, SW_DIGITAL_PIN_TIM_CHANNEL_SYNC_1);

    LL_TIM_SetTriggerOutput(SW_DIGITAL_PIN_TIM_TX, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(SW_DIGITAL_PIN_TIM_TX);

    // Configure Interrupt Timer
    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_TIM_IRQ, furi_hal_sw_digital_pin_tx_timer_isr, NULL);

    // Start
    LL_TIM_EnableDMAReq_UPDATE(SW_DIGITAL_PIN_TIM_TX);
    LL_TIM_GenerateEvent_UPDATE(SW_DIGITAL_PIN_TIM_TX);

    LL_TIM_SetCounter(SW_DIGITAL_PIN_TIM_TX, 0);
    LL_TIM_EnableCounter(SW_DIGITAL_PIN_TIM_TX);

    // Configure DMA
    LL_DMA_SetMemoryAddress(
        SW_DIGITAL_PIN_DMA_DEF_TX, (uint32_t)furi_hal_sw_digital_pin_buff.buffer_tx_ptr);
    LL_DMA_SetPeriphAddress(SW_DIGITAL_PIN_DMA_DEF_TX, (uint32_t) & (gpio->port->BSRR));
    LL_DMA_ConfigTransfer(
        SW_DIGITAL_PIN_DMA_DEF_TX,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD |
            LL_DMA_PRIORITY_HIGH);
    LL_DMA_SetDataLength(SW_DIGITAL_PIN_DMA_DEF_TX, furi_hal_sw_digital_pin_buff.buffer_tx_size);
    LL_DMA_SetPeriphRequest(SW_DIGITAL_PIN_DMA_DEF_TX, LL_DMAMUX_REQ_TIM2_CH1);

    // Configure DMA Channel CC1
    LL_TIM_EnableDMAReq_CC1(SW_DIGITAL_PIN_TIM_TX);
    LL_TIM_CC_EnableChannel(SW_DIGITAL_PIN_TIM_TX, SW_DIGITAL_PIN_TIM_CHANNEL_TX);

    // Start DMA irq
    furi_hal_interrupt_set_isr(
        SW_DIGITAL_PIN_DMA_IRQ_TX, furi_hal_sw_digital_pin_dma_tx_isr, NULL);
}

void furi_hal_sw_digital_pin_tx_start(void) {
    FURI_CRITICAL_ENTER();
    furi_hal_sw_digital_pin.state = SwDigitalPinStateTx;

    LL_DMA_SetDataLength(SW_DIGITAL_PIN_DMA_DEF_TX, furi_hal_sw_digital_pin_buff.buffer_tx_size);
#if SW_DIGITAL_PIN_DMA_CHANNEL_TX == LL_DMA_CHANNEL_1
    // Need to clear flags before enabling DMA !!!!
    if(LL_DMA_IsActiveFlag_HT1(SW_DIGITAL_PIN_DMA)) LL_DMA_ClearFlag_HT1(SW_DIGITAL_PIN_DMA);
    if(LL_DMA_IsActiveFlag_TC1(SW_DIGITAL_PIN_DMA)) LL_DMA_ClearFlag_TC1(SW_DIGITAL_PIN_DMA);
    if(LL_DMA_IsActiveFlag_TE1(SW_DIGITAL_PIN_DMA)) LL_DMA_ClearFlag_TE1(SW_DIGITAL_PIN_DMA);
#else
#error Update this code. Would you kindly?
#endif
    LL_DMA_EnableIT_TC(SW_DIGITAL_PIN_DMA_DEF_TX);
    LL_DMA_EnableIT_HT(SW_DIGITAL_PIN_DMA_DEF_TX);

    furi_hal_sw_digital_pin_buff_tx_refill(
        furi_hal_sw_digital_pin_buff.buffer_tx_ptr,
        furi_hal_sw_digital_pin_buff.buffer_tx_size,
        0);

    // Sync timer Channel Tx
    LL_TIM_OC_SetCompareCH1(SW_DIGITAL_PIN_TIM_TX, LL_TIM_GetCounter(SW_DIGITAL_PIN_TIM_TX));
#ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_TX
    furi_hal_gpio_write(gpio_debug_tx, true);
#endif
    LL_DMA_EnableChannel(SW_DIGITAL_PIN_DMA_DEF_TX);
    FURI_CRITICAL_EXIT();
}

bool furi_hal_sw_digital_pin_is_tx_complete(void) {
    return furi_hal_sw_digital_pin.state == SwDigitalPinStateTxEnd;
}

void furi_hal_sw_digital_pin_tx_stop(void) {
    FURI_CRITICAL_ENTER();
    if(LL_DMA_IsEnabledChannel(SW_DIGITAL_PIN_DMA_DEF_TX))
        LL_DMA_DisableChannel(SW_DIGITAL_PIN_DMA_DEF_TX);
    if(LL_DMA_IsEnabledIT_HT(SW_DIGITAL_PIN_DMA_DEF_TX))
        LL_DMA_DisableIT_HT(SW_DIGITAL_PIN_DMA_DEF_TX);
    if(LL_DMA_IsEnabledIT_TC(SW_DIGITAL_PIN_DMA_DEF_TX))
        LL_DMA_DisableIT_TC(SW_DIGITAL_PIN_DMA_DEF_TX);
    FURI_CRITICAL_EXIT();
}

void furi_hal_sw_digital_pin_tx_deinit(void) {
    furi_assert(
        furi_hal_sw_digital_pin.state == SwDigitalPinStateTx ||
        furi_hal_sw_digital_pin.state == SwDigitalPinStateTxLast ||
        furi_hal_sw_digital_pin.state == SwDigitalPinStateTxEnd);

    furi_hal_sw_digital_pin_tx_stop();

    FURI_CRITICAL_ENTER();
    // DeInit DMA Channel Tx
    // disable DMA Channel CC1
    LL_TIM_DisableDMAReq_CC1(SW_DIGITAL_PIN_TIM_TX);
    LL_TIM_CC_DisableChannel(SW_DIGITAL_PIN_TIM_TX, SW_DIGITAL_PIN_TIM_CHANNEL_TX);

    // DeInit DMA
    LL_DMA_DeInit(SW_DIGITAL_PIN_DMA_DEF_TX);

    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_DMA_IRQ_TX, NULL, NULL);

    // DeInit TIM
    LL_TIM_DisableCounter(SW_DIGITAL_PIN_TIM_TX);
    LL_TIM_DisableDMAReq_UPDATE(SW_DIGITAL_PIN_TIM_TX);
    LL_TIM_DeInit(SW_DIGITAL_PIN_TIM_TX);
    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_TIM_IRQ, NULL, NULL);
    FURI_CRITICAL_EXIT();

    // Deinitialize GPIO
#ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_TX
    furi_hal_gpio_init(gpio_debug_tx, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
#endif
    furi_hal_gpio_init(
        furi_hal_sw_digital_pin_buff.tx_gpio, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    free(furi_hal_sw_digital_pin_buff.buffer_tx_ptr);

    //furi_hal_sw_digital_pin.state = SwDigitalPinStateIdle;
}

void furi_hal_sw_digital_pin_rx_init(
    FuriHalSwDigitalPinRxCallback rx_callback,
    void* context,
    uint16_t tim_psc,
    uint32_t tim_arr,
    size_t samples,
    const GpioPin* gpio) {
    furi_assert(samples > 0);
    furi_assert(!(samples & 0x1));

    furi_hal_sw_digital_pin_buff.rx_gpio = gpio;
    furi_hal_sw_digital_pin_buff.buffer_rx_size = samples;
    furi_hal_sw_digital_pin_buff.buffer_rx_half_size =
        furi_hal_sw_digital_pin_buff.buffer_rx_size / 2;
    furi_hal_sw_digital_pin_buff.buffer_rx_ptr = malloc(samples * sizeof(uint16_t));

    furi_hal_sw_digital_pin_buff.rx_callback = rx_callback;
    furi_hal_sw_digital_pin_buff.rx_context = context;

    furi_hal_sw_digital_pin_buff.tim_arr_reset = 0;

#ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_RX
    furi_hal_gpio_init(gpio_debug_rx, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(gpio_debug_rx, false);
#endif

    // Configure TIM
    LL_TIM_SetPrescaler(SW_DIGITAL_PIN_TIM_RX, tim_psc);
    LL_TIM_SetCounterMode(SW_DIGITAL_PIN_TIM_RX, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(SW_DIGITAL_PIN_TIM_RX, tim_arr);
    LL_TIM_SetClockDivision(SW_DIGITAL_PIN_TIM_RX, LL_TIM_CLOCKDIVISION_DIV1);

    LL_TIM_DisableARRPreload(SW_DIGITAL_PIN_TIM_RX);
    LL_TIM_SetClockSource(SW_DIGITAL_PIN_TIM_RX, LL_TIM_CLOCKSOURCE_INTERNAL);

    // Configure TIM channel CC1
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = tim_arr / 2;
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    LL_TIM_OC_Init(SW_DIGITAL_PIN_TIM_RX, SW_DIGITAL_PIN_TIM_CHANNEL_SYNC_RX, &TIM_OC_InitStruct);
    //LL_TIM_OC_SetCompareCH1(TIMx, TIM_OCInitStruct->CompareValue);
    LL_TIM_OC_DisableFast(SW_DIGITAL_PIN_TIM_RX, SW_DIGITAL_PIN_TIM_CHANNEL_SYNC_RX);

    LL_TIM_SetTriggerOutput(SW_DIGITAL_PIN_TIM_RX, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(SW_DIGITAL_PIN_TIM_RX);

    // Start
    LL_TIM_GenerateEvent_UPDATE(SW_DIGITAL_PIN_TIM_RX);

    // Config DMA Sync timer

    /* We need the EXTI to be configured as interrupt generating line, but no ISR registered */
    furi_hal_gpio_init(gpio, GpioModeInterruptRiseFall, GpioPullUp, GpioSpeedVeryHigh);

    /* Set DMAMUX request generation signal ID on specified DMAMUX channel */
    LL_DMAMUX_SetRequestSignalID(DMAMUX1, LL_DMAMUX_REQ_GEN_0, GET_DMAMUX_EXTI_LINE(gpio->pin));
    /* Set the polarity of the signal on which the DMA request is generated */
    LL_DMAMUX_SetRequestGenPolarity(DMAMUX1, LL_DMAMUX_REQ_GEN_0, LL_DMAMUX_REQ_GEN_POL_RISING);
    /* Set the number of DMA requests that will be authorized after a generation event */
    LL_DMAMUX_SetGenRequestNb(DMAMUX1, LL_DMAMUX_REQ_GEN_0, 1);

    // Configure DMA Sync
    LL_DMA_SetMemoryAddress(
        SW_DIGITAL_PIN_DMA_SYNC_RX_DEF, (uint32_t)furi_hal_sw_digital_pin_buff.tim_arr_reset);
    LL_DMA_SetPeriphAddress(
        SW_DIGITAL_PIN_DMA_SYNC_RX_DEF, (uint32_t) & (SW_DIGITAL_PIN_TIM_RX->CNT));
    LL_DMA_ConfigTransfer(
        SW_DIGITAL_PIN_DMA_SYNC_RX_DEF,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_NOINCREMENT | LL_DMA_PDATAALIGN_HALFWORD | LL_DMA_MDATAALIGN_HALFWORD |
            LL_DMA_PRIORITY_VERYHIGH);
    LL_DMA_SetDataLength(SW_DIGITAL_PIN_DMA_SYNC_RX_DEF, 1);
    LL_DMA_SetPeriphRequest(SW_DIGITAL_PIN_DMA_SYNC_RX_DEF, LL_DMAMUX_REQ_GENERATOR0);

    // Configure DMA Rx pin
    LL_DMA_SetMemoryAddress(
        SW_DIGITAL_PIN_DMA_DEF_RX, (uint32_t)furi_hal_sw_digital_pin_buff.buffer_rx_ptr);
    LL_DMA_SetPeriphAddress(SW_DIGITAL_PIN_DMA_DEF_RX, (uint32_t) & (gpio->port->IDR));
    LL_DMA_ConfigTransfer(
        SW_DIGITAL_PIN_DMA_DEF_RX,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_INCREMENT | LL_DMA_PDATAALIGN_HALFWORD | LL_DMA_MDATAALIGN_HALFWORD |
            LL_DMA_PRIORITY_VERYHIGH);
    LL_DMA_SetDataLength(SW_DIGITAL_PIN_DMA_DEF_RX, furi_hal_sw_digital_pin_buff.buffer_rx_size);
    LL_DMA_SetPeriphRequest(SW_DIGITAL_PIN_DMA_DEF_RX, LL_DMAMUX_REQ_TIM17_CH1);

    // Configure DMA Channel CC1
    LL_TIM_EnableDMAReq_CC1(SW_DIGITAL_PIN_TIM_RX);
    LL_TIM_CC_EnableChannel(SW_DIGITAL_PIN_TIM_RX, SW_DIGITAL_PIN_TIM_CHANNEL_SYNC_RX);

    // Start DMA irq, higher priority than normal
    furi_hal_interrupt_set_isr_ex(
        SW_DIGITAL_PIN_DMA_IRQ_RX, 4, furi_hal_sw_digital_pin_dma_rx_isr, NULL);
#if SW_DIGITAL_PIN_DMA_CHANNEL_RX == LL_DMA_CHANNEL_2
    // Need to clear flags before enabling DMA !!!!
    if(LL_DMA_IsActiveFlag_HT2(SW_DIGITAL_PIN_DMA)) LL_DMA_ClearFlag_HT1(SW_DIGITAL_PIN_DMA);
    if(LL_DMA_IsActiveFlag_TC2(SW_DIGITAL_PIN_DMA)) LL_DMA_ClearFlag_TC1(SW_DIGITAL_PIN_DMA);
    if(LL_DMA_IsActiveFlag_TE2(SW_DIGITAL_PIN_DMA)) LL_DMA_ClearFlag_TE1(SW_DIGITAL_PIN_DMA);
#else
#error Update this code. Would you kindly?
#endif
    LL_DMA_EnableIT_TC(SW_DIGITAL_PIN_DMA_DEF_RX);
    LL_DMA_EnableIT_HT(SW_DIGITAL_PIN_DMA_DEF_RX);
}

void furi_hal_sw_digital_pin_rx_start(void) {
    FURI_CRITICAL_ENTER();

    LL_DMA_SetDataLength(SW_DIGITAL_PIN_DMA_DEF_RX, furi_hal_sw_digital_pin_buff.buffer_rx_size);

    // Start DMA Sync timer
    LL_DMA_EnableChannel(SW_DIGITAL_PIN_DMA_SYNC_RX_DEF);
    LL_DMAMUX_EnableRequestGen(DMAMUX1, LL_DMAMUX_REQ_GEN_0);

#ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_RX
    furi_hal_gpio_write(gpio_debug_rx, true);
#endif

    // Start DMA Rx pin
    LL_DMA_EnableChannel(SW_DIGITAL_PIN_DMA_DEF_RX);
    // Strat timer
    LL_TIM_SetCounter(SW_DIGITAL_PIN_TIM_RX, 0);
    LL_TIM_EnableCounter(SW_DIGITAL_PIN_TIM_RX);
    FURI_CRITICAL_EXIT();
}

void furi_hal_sw_digital_pin_rx_stop(void) {
    FURI_CRITICAL_ENTER();
    // Stop timer
    if(LL_TIM_IsEnabledCounter(SW_DIGITAL_PIN_TIM_RX))
        LL_TIM_DisableCounter(SW_DIGITAL_PIN_TIM_RX);
    // Stop DMA Rx pin
    if(LL_DMA_IsEnabledChannel(SW_DIGITAL_PIN_DMA_DEF_RX))
        LL_DMA_DisableChannel(SW_DIGITAL_PIN_DMA_DEF_RX);
    // Stop DMA Sync timer
    if(LL_DMA_IsEnabledChannel(SW_DIGITAL_PIN_DMA_SYNC_RX_DEF))
        LL_DMA_DisableChannel(SW_DIGITAL_PIN_DMA_SYNC_RX_DEF);
    if(LL_DMAMUX_IsEnabledRequestGen(DMAMUX1, LL_DMAMUX_REQ_GEN_0))
        LL_DMAMUX_DisableRequestGen(DMAMUX1, LL_DMAMUX_REQ_GEN_0);
    // Stop DMA irq
    if(LL_DMA_IsEnabledIT_HT(SW_DIGITAL_PIN_DMA_DEF_RX))
        LL_DMA_DisableIT_HT(SW_DIGITAL_PIN_DMA_DEF_RX);
    if(LL_DMA_IsEnabledIT_TC(SW_DIGITAL_PIN_DMA_DEF_RX))
        LL_DMA_DisableIT_TC(SW_DIGITAL_PIN_DMA_DEF_RX);
    FURI_CRITICAL_EXIT();
#ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_RX
    furi_hal_gpio_write(gpio_debug_rx, false);
#endif
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
    LL_TIM_DeInit(SW_DIGITAL_PIN_TIM_RX);
    LL_TIM_DisableDMAReq_UPDATE(SW_DIGITAL_PIN_TIM_RX);

    // Deinit DMA Rx pin
    LL_DMA_DeInit(SW_DIGITAL_PIN_DMA_DEF_RX);
    // Deinit DMA Sync timer
    LL_DMA_DeInit(SW_DIGITAL_PIN_DMA_SYNC_RX_DEF);

    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_DMA_IRQ_RX, NULL, NULL);
    FURI_CRITICAL_EXIT();

    // Deinitialize GPIO
    furi_hal_gpio_remove_int_callback(furi_hal_sw_digital_pin_buff.rx_gpio);
#ifdef FURI_HAL_SW_DIGITAL_DEBUG_PIN_RX

    furi_hal_gpio_init(gpio_debug_rx, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
#endif

    free(furi_hal_sw_digital_pin_buff.buffer_rx_ptr);

    //furi_hal_sw_digital_pin.state = SwDigitalPinStateIdle;
}