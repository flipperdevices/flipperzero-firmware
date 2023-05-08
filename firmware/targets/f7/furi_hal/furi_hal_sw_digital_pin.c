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
#define SW_DIGITAL_PIN_DMA_CH1_CHANNEL LL_DMA_CHANNEL_1
#define SW_DIGITAL_PIN_DMA_CH2_CHANNEL LL_DMA_CHANNEL_2
#define SW_DIGITAL_PIN_DMA_CH1_IRQ FuriHalInterruptIdDma2Ch1
#define SW_DIGITAL_PIN_DMA_CH2_IRQ FuriHalInterruptIdDma2Ch2
#define SW_DIGITAL_PIN_DMA_CH1_DEF SW_DIGITAL_PIN_DMA, SW_DIGITAL_PIN_DMA_CH1_CHANNEL
#define SW_DIGITAL_PIN_DMA_CH2_DEF SW_DIGITAL_PIN_DMA, SW_DIGITAL_PIN_DMA_CH2_CHANNEL

// #define SW_DIGITAL_PIN_BUFFER_FULL (24)
// #define SW_DIGITAL_PIN_BUFFER_HALF (SW_DIGITAL_PIN_BUFFER_FULL / 2)

typedef struct {
    uint32_t* buffer_tx_ptr;
    uint32_t* buffer_rx_ptr;
    size_t buffer_size;
    size_t buffer_half_size;
    // LevelDuration carry_ld;
    FuriHalSwDigitalPinTxCallback tx_callback;
    void* tx_context;
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

void furi_hal_sw_digital_pin_init(uint16_t psc, uint16_t arr) {
    furi_assert(furi_hal_sw_digital_pin.state == SwDigitalPinStateInit);


  /* Initialize TIM peripheral as follow:
  + Period = TimerPeriod 
  + Prescaler = 0
  + ClockDivision = 0
  + Counter direction = Up
  */

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = psc;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = arr;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(SW_DIGITAL_PIN_TIM, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(SW_DIGITAL_PIN_TIM);
    LL_TIM_SetClockSource(SW_DIGITAL_PIN_TIM, LL_TIM_CLOCKSOURCE_INTERNAL);

    furi_hal_sw_digital_pin.state = SwDigitalPinStateIdle;
}

static void furi_hal_sw_digital_pin_buff_tx_refill(uint32_t* buffer, size_t samples) {
    furi_assert(furi_hal_sw_digital_pin.state == SwDigitalPinStateTx);

    while(samples > 0) {
        volatile uint32_t data =
            furi_hal_sw_digital_pin_buff.tx_callback(furi_hal_sw_digital_pin_buff.tx_context);

        if(data == 0) {
            *buffer = 0;
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
        if(LL_TIM_GetAutoReload(SW_DIGITAL_PIN_TIM) == 0) {
            if(furi_hal_sw_digital_pin.state == SwDigitalPinStateTx) {
                furi_hal_sw_digital_pin.state = SwDigitalPinStateTxLast;
                LL_DMA_DisableChannel(SW_DIGITAL_PIN_DMA_CH1_DEF);
            } else if(furi_hal_sw_digital_pin.state == SwDigitalPinStateTxLast) {
                furi_hal_sw_digital_pin.state = SwDigitalPinStateTxEnd;
                //forcibly pulls the pin to the ground so that there is no carrier
                //furi_hal_gpio_init(&gpio_cc1101_g0, GpioModeInput, GpioPullDown, GpioSpeedLow);
                // No need off???
                LL_TIM_DisableCounter(SW_DIGITAL_PIN_TIM);
            } else {
                furi_crash(NULL);
            }
        }
    }
}

static void furi_hal_sw_digital_pin_dma_tx_isr() {
    furi_assert(furi_hal_sw_digital_pin.state == SwDigitalPinStateTx);

#if SW_DIGITAL_PIN_DMA_CH1_CHANNEL == LL_DMA_CHANNEL_1
    if(LL_DMA_IsActiveFlag_HT1(SW_DIGITAL_PIN_DMA)) {
        LL_DMA_ClearFlag_HT1(SW_DIGITAL_PIN_DMA);

        furi_hal_gpio_write(&gpio_ext_pa6, true);
        furi_hal_sw_digital_pin_buff_tx_refill(
            furi_hal_sw_digital_pin_buff.buffer_tx_ptr,
            furi_hal_sw_digital_pin_buff.buffer_half_size);
    }
    if(LL_DMA_IsActiveFlag_TC1(SW_DIGITAL_PIN_DMA)) {
        LL_DMA_ClearFlag_TC1(SW_DIGITAL_PIN_DMA);

        furi_hal_gpio_write(&gpio_ext_pa6, false);
        furi_hal_sw_digital_pin_buff_tx_refill(
            furi_hal_sw_digital_pin_buff.buffer_tx_ptr +
                furi_hal_sw_digital_pin_buff.buffer_half_size,
            furi_hal_sw_digital_pin_buff.buffer_half_size);
    }
#else
#error Update this code. Would you kindly?
#endif
}
\
void furi_hal_sw_digital_pin_tx_start(
    FuriHalSwDigitalPinTxCallback callback,
    void* context,
    size_t samples,
    const GpioPin* gpio) {
    //too many asserts
    furi_assert(callback);
    furi_assert(samples > 0);

    UNUSED(gpio);

    furi_hal_sw_digital_pin.state = SwDigitalPinStateTx;

    furi_hal_sw_digital_pin_buff.buffer_size = samples;
    furi_hal_sw_digital_pin_buff.buffer_half_size = furi_hal_sw_digital_pin_buff.buffer_size / 2;
    furi_hal_sw_digital_pin_buff.tx_callback = callback;
    furi_hal_sw_digital_pin_buff.tx_context = context;
    furi_hal_sw_digital_pin_buff.buffer_tx_ptr =
        malloc(furi_hal_sw_digital_pin_buff.buffer_size * sizeof(uint32_t));

    furi_hal_gpio_init(&gpio_ext_pa6, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);


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
    dma_config.NbData = furi_hal_sw_digital_pin_buff.buffer_size;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    dma_config.Priority = LL_DMA_PRIORITY_VERYHIGH;
    LL_DMA_Init(SW_DIGITAL_PIN_DMA_CH1_DEF, &dma_config);
    furi_hal_interrupt_set_isr(
        SW_DIGITAL_PIN_DMA_CH1_IRQ, furi_hal_sw_digital_pin_dma_tx_isr, NULL);
    LL_DMA_EnableIT_TC(SW_DIGITAL_PIN_DMA_CH1_DEF);
    LL_DMA_EnableIT_HT(SW_DIGITAL_PIN_DMA_CH1_DEF);
    LL_DMA_EnableChannel(SW_DIGITAL_PIN_DMA_CH1_DEF);

    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_TIM_IRQ, furi_hal_sw_digital_pin_tx_timer_isr, NULL);
    furi_hal_sw_digital_pin_buff_tx_refill(
        furi_hal_sw_digital_pin_buff.buffer_tx_ptr, furi_hal_sw_digital_pin_buff.buffer_size);

    //start
    LL_TIM_EnableDMAReq_UPDATE(SW_DIGITAL_PIN_TIM);
    LL_TIM_GenerateEvent_UPDATE(SW_DIGITAL_PIN_TIM);
    LL_TIM_CC_EnableChannel(SW_DIGITAL_PIN_TIM, LL_TIM_CHANNEL_CH1);
    
    LL_TIM_SetCounter(SW_DIGITAL_PIN_TIM, 0);
    LL_TIM_EnableCounter(SW_DIGITAL_PIN_TIM);
}

bool furi_hal_sw_digital_pin_is_tx_complete(void) {
    return furi_hal_sw_digital_pin.state == SwDigitalPinStateTxEnd;
}

void furi_hal_sw_digital_pin_tx_stop(void) {
    furi_assert(
        furi_hal_sw_digital_pin.state == SwDigitalPinStateTx ||
        furi_hal_sw_digital_pin.state == SwDigitalPinStateTxLast ||
        furi_hal_sw_digital_pin.state == SwDigitalPinStateTxEnd);

    // Deinitialize Timer
    FURI_CRITICAL_ENTER();
    LL_TIM_DeInit(SW_DIGITAL_PIN_TIM);
    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_TIM_IRQ, NULL, NULL);

    // Deinitialize DMA
    LL_DMA_DeInit(SW_DIGITAL_PIN_DMA_CH1_DEF);

    furi_hal_interrupt_set_isr(SW_DIGITAL_PIN_DMA_CH1_IRQ, NULL, NULL);

    // Deinitialize GPIO
    furi_hal_gpio_init(&gpio_ext_pa6, GpioModeAnalog, GpioPullNo, GpioSpeedLow);

    FURI_CRITICAL_EXIT();

    free(furi_hal_sw_digital_pin_buff.buffer_tx_ptr);

    furi_hal_sw_digital_pin.state = SwDigitalPinStateIdle;
}