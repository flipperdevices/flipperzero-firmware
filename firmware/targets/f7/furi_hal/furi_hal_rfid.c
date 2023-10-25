#include <furi_hal_rfid.h>
#include <furi_hal_ibutton.h>
#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>
#include <furi_hal_bus.h>
#include <furi.h>

#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_comp.h>
#include <stm32wbxx_ll_dma.h>

// TIMER definitions
#define FURI_HAL_RFID_READ_TIMER TIM1
#define FURI_HAL_RFID_READ_TIMER_BUS FuriHalBusTIM1
#define FURI_HAL_RFID_READ_TIMER_CHANNEL LL_TIM_CHANNEL_CH1N
// We can't use N channel for LL_TIM_OC_Init, so...
#define FURI_HAL_RFID_READ_TIMER_CHANNEL_CONFIG LL_TIM_CHANNEL_CH1

#define FURI_HAL_RFID_EMULATE_TIMER TIM2
#define FURI_HAL_RFID_EMULATE_TIMER_BUS FuriHalBusTIM2
#define FURI_HAL_RFID_EMULATE_TIMER_IRQ FuriHalInterruptIdTIM2
#define FURI_HAL_RFID_EMULATE_TIMER_CHANNEL LL_TIM_CHANNEL_CH3

#define FURI_HAL_RFID_RTF_PULL_OUT_TIMER TIM2
//#define FURI_HAL_RFID_RTF_PULL_OUT_TIMER_BUS FuriHalBusTIM2
#define FURI_HAL_RFID_RTF_PULL_OUT_TIMER_CHANNEL LL_TIM_CHANNEL_CH3

#define FURI_HAL_RFID_RTF_CARRIER_IN_TIMER TIM2
#define FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_BUS FuriHalBusTIM2
#define FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_DIR_CH LL_TIM_CHANNEL_CH1

#define FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER TIM1
#define FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER_BUS FuriHalBusTIM1

#define RFID_CAPTURE_TIM TIM2
#define RFID_CAPTURE_TIM_BUS FuriHalBusTIM2
#define RFID_CAPTURE_IND_CH LL_TIM_CHANNEL_CH3
#define RFID_CAPTURE_DIR_CH LL_TIM_CHANNEL_CH4

#define CARRIER_OUT_TIMER TIM1
#define CARRIER_OUT_TIMER_BUS FuriHalBusTIM1
#define CARRIER_OUT_TIMER_CHANNEL LL_TIM_CHANNEL_CH1 // or LL_TIM_CHANNEL_CH1N

// Field presence detection
#define FURI_HAL_RFID_FIELD_FREQUENCY_MIN 80000
#define FURI_HAL_RFID_FIELD_FREQUENCY_MAX 200000

#define FURI_HAL_RFID_FIELD_COUNTER_TIMER TIM2
#define FURI_HAL_RFID_FIELD_COUNTER_TIMER_BUS FuriHalBusTIM2
#define FURI_HAL_RFID_FIELD_COUNTER_TIMER_CHANNEL LL_TIM_CHANNEL_CH3

#define FURI_HAL_RFID_FIELD_TIMEOUT_TIMER TIM1
#define FURI_HAL_RFID_FIELD_TIMEOUT_TIMER_BUS FuriHalBusTIM1

#define FURI_HAL_RFID_FIELD_DMAMUX_DMA LL_DMAMUX_REQ_TIM1_UP

/* DMA Channels definition */
#define RFID_DMA DMA2
#define RFID_DMA_CH1_CHANNEL LL_DMA_CHANNEL_1
#define RFID_DMA_CH2_CHANNEL LL_DMA_CHANNEL_2
#define RFID_DMA_CH1_IRQ FuriHalInterruptIdDma2Ch1
#define RFID_DMA_CH1_DEF RFID_DMA, RFID_DMA_CH1_CHANNEL
#define RFID_DMA_CH2_DEF RFID_DMA, RFID_DMA_CH2_CHANNEL

// DMA Channels definition for RTF mode
#define FURI_HAL_RFID_RTF_PULL_OUT_DMA DMA2
#define FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1 LL_DMA_CHANNEL_1
#define FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH2 LL_DMA_CHANNEL_2
#define FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1_IRQ FuriHalInterruptIdDma2Ch1
#define FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1_DEF FURI_HAL_RFID_RTF_PULL_OUT_DMA, FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1
#define FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH2_DEF FURI_HAL_RFID_RTF_PULL_OUT_DMA, FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH2


typedef struct {
    uint32_t counter;
    uint32_t set_tim_counter_cnt;
} FuriHalRfidField;

typedef struct {
    FuriHalRfidDMACallback dma_callback;
    FuriHalRfidReadCaptureCallback read_capture_callback;
    void* context;
    FuriHalRfidField field;
	uint32_t prevTIMval;
} FuriHalRfid;

FuriHalRfid* furi_hal_rfid = NULL;

#define LFRFID_LL_READ_TIM TIM1
#define LFRFID_LL_READ_CONFIG_CHANNEL LL_TIM_CHANNEL_CH1
#define LFRFID_LL_READ_CHANNEL LL_TIM_CHANNEL_CH1N

#define LFRFID_LL_EMULATE_TIM TIM2
#define LFRFID_LL_EMULATE_CHANNEL LL_TIM_CHANNEL_CH3

void furi_hal_rfid_init() {
    furi_assert(furi_hal_rfid == NULL);
    furi_hal_rfid = malloc(sizeof(FuriHalRfid));
    furi_hal_rfid->field.counter = 0;
    furi_hal_rfid->field.set_tim_counter_cnt = 0;
	furi_hal_rfid->prevTIMval = 0;

    furi_hal_rfid_pins_reset();

    LL_COMP_InitTypeDef COMP_InitStruct = {0};
    COMP_InitStruct.PowerMode = LL_COMP_POWERMODE_MEDIUMSPEED;
    COMP_InitStruct.InputPlus = LL_COMP_INPUT_PLUS_IO1;
    COMP_InitStruct.InputMinus = LL_COMP_INPUT_MINUS_1_2VREFINT;
    COMP_InitStruct.InputHysteresis = LL_COMP_HYSTERESIS_HIGH;
    COMP_InitStruct.OutputPolarity = LL_COMP_OUTPUTPOL_NONINVERTED;
    COMP_InitStruct.OutputBlankingSource = LL_COMP_BLANKINGSRC_NONE;
    LL_COMP_Init(COMP1, &COMP_InitStruct);
    LL_COMP_SetCommonWindowMode(__LL_COMP_COMMON_INSTANCE(COMP1), LL_COMP_WINDOWMODE_DISABLE);

    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_20);
    LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_20);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_20);
    LL_EXTI_DisableEvent_0_31(LL_EXTI_LINE_20);
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_20);

    NVIC_SetPriority(COMP_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(COMP_IRQn);
}

void furi_hal_rfid_pins_reset() {
    // ibutton bus disable
    furi_hal_ibutton_pin_reset();

    // pulldown rfid antenna
    furi_hal_gpio_init(&gpio_rfid_carrier_out, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_rfid_carrier_out, false);

    // from both sides
    furi_hal_gpio_init(&gpio_nfc_irq_rfid_pull, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_nfc_irq_rfid_pull, true);

    furi_hal_gpio_init_simple(&gpio_rfid_carrier, GpioModeAnalog);

    furi_hal_gpio_init(&gpio_rfid_data_in, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

static void furi_hal_rfid_pins_emulate() {
    // ibutton low
    furi_hal_ibutton_pin_configure();
    furi_hal_ibutton_pin_write(false);

    // pull pin to timer out
    furi_hal_gpio_init_ex(
        &gpio_nfc_irq_rfid_pull,
        GpioModeAltFunctionPushPull,
        GpioPullNo,
        GpioSpeedLow,
        GpioAltFn1TIM2);

    // pull rfid antenna from carrier side
    furi_hal_gpio_init(&gpio_rfid_carrier_out, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_rfid_carrier_out, false);

    furi_hal_gpio_init_ex(
        &gpio_rfid_carrier, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn2TIM2);
}

static void furi_hal_rfid_pins_read() {
    // ibutton low
    furi_hal_ibutton_pin_configure();
    furi_hal_ibutton_pin_write(false);

    // dont pull rfid antenna
    furi_hal_gpio_init(&gpio_nfc_irq_rfid_pull, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_nfc_irq_rfid_pull, false);

    // carrier pin to timer out
    furi_hal_gpio_init_ex(
        &gpio_rfid_carrier_out,
        GpioModeAltFunctionPushPull,
        GpioPullNo,
        GpioSpeedLow,
        GpioAltFn1TIM1);

    // comparator in
    furi_hal_gpio_init(&gpio_rfid_data_in, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

static void furi_hal_rfid_pins_field() {
    // ibutton low
    furi_hal_ibutton_pin_configure();
    furi_hal_ibutton_pin_write(false);

    // pull pin to timer out
    furi_hal_gpio_init(&gpio_nfc_irq_rfid_pull, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_nfc_irq_rfid_pull, false);

    // pull rfid antenna from carrier side
    furi_hal_gpio_init(&gpio_rfid_carrier_out, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_rfid_carrier_out, false);

    furi_hal_gpio_init_ex(
        &gpio_rfid_carrier, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn2TIM2);
}

void furi_hal_rfid_pin_pull_release() {
    furi_hal_gpio_write(&gpio_nfc_irq_rfid_pull, true);
}

void furi_hal_rfid_pin_pull_pulldown() {
    furi_hal_gpio_write(&gpio_nfc_irq_rfid_pull, false);
}

void furi_hal_rfid_tim_read_start(float freq, float duty_cycle) {
    furi_hal_bus_enable(FURI_HAL_RFID_READ_TIMER_BUS);

    furi_hal_rfid_pins_read();

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Autoreload = (SystemCoreClock / freq) - 1;
    LL_TIM_Init(FURI_HAL_RFID_READ_TIMER, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(FURI_HAL_RFID_READ_TIMER);

    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_ENABLE;
    TIM_OC_InitStruct.CompareValue = TIM_InitStruct.Autoreload * duty_cycle;
    LL_TIM_OC_Init(
        FURI_HAL_RFID_READ_TIMER, FURI_HAL_RFID_READ_TIMER_CHANNEL_CONFIG, &TIM_OC_InitStruct);

    LL_TIM_EnableCounter(FURI_HAL_RFID_READ_TIMER);

    furi_hal_rfid_tim_read_continue();
}

void furi_hal_rfid_tim_read_continue() {
    LL_TIM_EnableAllOutputs(FURI_HAL_RFID_READ_TIMER);
}

void furi_hal_rfid_tim_read_pause() {
    LL_TIM_DisableAllOutputs(FURI_HAL_RFID_READ_TIMER);
}

void furi_hal_rfid_tim_read_stop() {
    furi_hal_bus_disable(FURI_HAL_RFID_READ_TIMER_BUS);
}

static void furi_hal_rfid_tim_emulate() {
    LL_TIM_SetPrescaler(FURI_HAL_RFID_EMULATE_TIMER, 0);
    LL_TIM_SetCounterMode(FURI_HAL_RFID_EMULATE_TIMER, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(FURI_HAL_RFID_EMULATE_TIMER, 1);
    LL_TIM_DisableARRPreload(FURI_HAL_RFID_EMULATE_TIMER);
    LL_TIM_SetRepetitionCounter(FURI_HAL_RFID_EMULATE_TIMER, 0);

    LL_TIM_SetClockDivision(FURI_HAL_RFID_EMULATE_TIMER, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetClockSource(FURI_HAL_RFID_EMULATE_TIMER, LL_TIM_CLOCKSOURCE_EXT_MODE2);
    LL_TIM_ConfigETR(
        FURI_HAL_RFID_EMULATE_TIMER,
        LL_TIM_ETR_POLARITY_INVERTED,
        LL_TIM_ETR_PRESCALER_DIV1,
        LL_TIM_ETR_FILTER_FDIV1);

    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
    TIM_OC_InitStruct.CompareValue = 1;
    LL_TIM_OC_Init(
        FURI_HAL_RFID_EMULATE_TIMER, FURI_HAL_RFID_EMULATE_TIMER_CHANNEL, &TIM_OC_InitStruct);

    LL_TIM_GenerateEvent_UPDATE(FURI_HAL_RFID_EMULATE_TIMER);
}

static void furi_hal_capture_dma_isr(void* context) {
    UNUSED(context);

    // Channel 3, positive level
    if(LL_TIM_IsActiveFlag_CC3(RFID_CAPTURE_TIM)) {
        LL_TIM_ClearFlag_CC3(RFID_CAPTURE_TIM);
        furi_hal_rfid->read_capture_callback(
            true, LL_TIM_IC_GetCaptureCH3(RFID_CAPTURE_TIM), furi_hal_rfid->context);
    }

    // Channel 4, overall level
    if(LL_TIM_IsActiveFlag_CC4(RFID_CAPTURE_TIM)) {
        LL_TIM_ClearFlag_CC4(RFID_CAPTURE_TIM);
        LL_TIM_SetCounter(RFID_CAPTURE_TIM, 0);
        furi_hal_rfid->read_capture_callback(
            false, LL_TIM_IC_GetCaptureCH4(RFID_CAPTURE_TIM), furi_hal_rfid->context);
    }
}

void furi_hal_rfid_tim_read_capture_start(FuriHalRfidReadCaptureCallback callback, void* context) {
    furi_assert(furi_hal_rfid);

    furi_hal_rfid->read_capture_callback = callback;
    furi_hal_rfid->context = context;

    furi_hal_bus_enable(RFID_CAPTURE_TIM_BUS);

    // Timer: base
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = 64 - 1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = UINT32_MAX;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(RFID_CAPTURE_TIM, &TIM_InitStruct);

    // Timer: advanced
    LL_TIM_SetClockSource(RFID_CAPTURE_TIM, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_DisableARRPreload(RFID_CAPTURE_TIM);
    LL_TIM_SetTriggerInput(RFID_CAPTURE_TIM, LL_TIM_TS_TI2FP2);
    LL_TIM_SetSlaveMode(RFID_CAPTURE_TIM, LL_TIM_SLAVEMODE_DISABLED);
    LL_TIM_SetTriggerOutput(RFID_CAPTURE_TIM, LL_TIM_TRGO_RESET);
    LL_TIM_EnableMasterSlaveMode(RFID_CAPTURE_TIM);
    LL_TIM_DisableDMAReq_TRIG(RFID_CAPTURE_TIM);
    LL_TIM_DisableIT_TRIG(RFID_CAPTURE_TIM);
    LL_TIM_SetRemap(RFID_CAPTURE_TIM, LL_TIM_TIM2_TI4_RMP_COMP1);

    // Timer: channel 3 indirect
    LL_TIM_IC_SetActiveInput(RFID_CAPTURE_TIM, RFID_CAPTURE_IND_CH, LL_TIM_ACTIVEINPUT_INDIRECTTI);
    LL_TIM_IC_SetPrescaler(RFID_CAPTURE_TIM, RFID_CAPTURE_IND_CH, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetPolarity(RFID_CAPTURE_TIM, RFID_CAPTURE_IND_CH, LL_TIM_IC_POLARITY_FALLING);
    LL_TIM_IC_SetFilter(RFID_CAPTURE_TIM, RFID_CAPTURE_IND_CH, LL_TIM_IC_FILTER_FDIV1);

    // Timer: channel 4 direct
    LL_TIM_IC_SetActiveInput(RFID_CAPTURE_TIM, RFID_CAPTURE_DIR_CH, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(RFID_CAPTURE_TIM, RFID_CAPTURE_DIR_CH, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetPolarity(RFID_CAPTURE_TIM, RFID_CAPTURE_DIR_CH, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_IC_SetFilter(RFID_CAPTURE_TIM, RFID_CAPTURE_DIR_CH, LL_TIM_IC_FILTER_FDIV1);

    furi_hal_interrupt_set_isr(FURI_HAL_RFID_EMULATE_TIMER_IRQ, furi_hal_capture_dma_isr, NULL);

    LL_TIM_EnableIT_CC3(RFID_CAPTURE_TIM);
    LL_TIM_EnableIT_CC4(RFID_CAPTURE_TIM);
    LL_TIM_CC_EnableChannel(RFID_CAPTURE_TIM, RFID_CAPTURE_IND_CH);
    LL_TIM_CC_EnableChannel(RFID_CAPTURE_TIM, RFID_CAPTURE_DIR_CH);
    LL_TIM_SetCounter(RFID_CAPTURE_TIM, 0);
    LL_TIM_EnableCounter(RFID_CAPTURE_TIM);

    furi_hal_rfid_comp_start();
}

void furi_hal_rfid_tim_read_capture_stop() {
    furi_hal_rfid_comp_stop();

    furi_hal_interrupt_set_isr(FURI_HAL_RFID_EMULATE_TIMER_IRQ, NULL, NULL);
    furi_hal_bus_disable(RFID_CAPTURE_TIM_BUS);
}

static void furi_hal_rfid_dma_isr() {
#if RFID_DMA_CH1_CHANNEL == LL_DMA_CHANNEL_1
    if(LL_DMA_IsActiveFlag_HT1(RFID_DMA)) {
        LL_DMA_ClearFlag_HT1(RFID_DMA);
        furi_hal_rfid->dma_callback(true, furi_hal_rfid->context);
    }

    if(LL_DMA_IsActiveFlag_TC1(RFID_DMA)) {
        LL_DMA_ClearFlag_TC1(RFID_DMA);
        furi_hal_rfid->dma_callback(false, furi_hal_rfid->context);
    }
#else
#error Update this code. Would you kindly?
#endif
}

void furi_hal_rfid_tim_emulate_dma_start(
    uint32_t* duration,
    uint32_t* pulse,
    size_t length,
    FuriHalRfidDMACallback callback,
    void* context) {
    furi_assert(furi_hal_rfid);

    // setup interrupts
    furi_hal_rfid->dma_callback = callback;
    furi_hal_rfid->context = context;

    // setup pins
    furi_hal_rfid_pins_emulate();

    // configure timer
    furi_hal_bus_enable(FURI_HAL_RFID_EMULATE_TIMER_BUS);
    furi_hal_rfid_tim_emulate();
    LL_TIM_OC_SetPolarity(
        FURI_HAL_RFID_EMULATE_TIMER, FURI_HAL_RFID_EMULATE_TIMER_CHANNEL, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_EnableDMAReq_UPDATE(FURI_HAL_RFID_EMULATE_TIMER);

    // configure DMA "mem -> ARR" channel
    LL_DMA_InitTypeDef dma_config = {0};
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (FURI_HAL_RFID_EMULATE_TIMER->ARR);
    dma_config.MemoryOrM2MDstAddress = (uint32_t)duration;
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.NbData = length;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    dma_config.Priority = LL_DMA_MODE_NORMAL;
    LL_DMA_Init(RFID_DMA_CH1_DEF, &dma_config);
    LL_DMA_EnableChannel(RFID_DMA_CH1_DEF);

    // configure DMA "mem -> CCR3" channel
#if FURI_HAL_RFID_EMULATE_TIMER_CHANNEL == LL_TIM_CHANNEL_CH3
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (FURI_HAL_RFID_EMULATE_TIMER->CCR3);
#else
#error Update this code. Would you kindly?
#endif
    dma_config.MemoryOrM2MDstAddress = (uint32_t)pulse;
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.NbData = length;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    dma_config.Priority = LL_DMA_MODE_NORMAL;
    LL_DMA_Init(RFID_DMA_CH2_DEF, &dma_config);
    LL_DMA_EnableChannel(RFID_DMA_CH2_DEF);

    // attach interrupt to one of DMA channels
    furi_hal_interrupt_set_isr(RFID_DMA_CH1_IRQ, furi_hal_rfid_dma_isr, NULL);
    LL_DMA_EnableIT_TC(RFID_DMA_CH1_DEF);
    LL_DMA_EnableIT_HT(RFID_DMA_CH1_DEF);

    // start
    LL_TIM_EnableAllOutputs(FURI_HAL_RFID_EMULATE_TIMER);

    LL_TIM_SetCounter(FURI_HAL_RFID_EMULATE_TIMER, 0);
    LL_TIM_EnableCounter(FURI_HAL_RFID_EMULATE_TIMER);
}

void furi_hal_rfid_tim_emulate_dma_stop() {
    LL_TIM_DisableCounter(FURI_HAL_RFID_EMULATE_TIMER);
    LL_TIM_DisableAllOutputs(FURI_HAL_RFID_EMULATE_TIMER);

    furi_hal_interrupt_set_isr(RFID_DMA_CH1_IRQ, NULL, NULL);
    LL_DMA_DisableIT_TC(RFID_DMA_CH1_DEF);
    LL_DMA_DisableIT_HT(RFID_DMA_CH1_DEF);

    FURI_CRITICAL_ENTER();

    LL_DMA_DeInit(RFID_DMA_CH1_DEF);
    LL_DMA_DeInit(RFID_DMA_CH2_DEF);

    furi_hal_bus_disable(FURI_HAL_RFID_EMULATE_TIMER_BUS);

    FURI_CRITICAL_EXIT();
}

void furi_hal_rfid_set_read_period(uint32_t period) {
    LL_TIM_SetAutoReload(FURI_HAL_RFID_READ_TIMER, period);
}

void furi_hal_rfid_set_read_pulse(uint32_t pulse) {
#if FURI_HAL_RFID_READ_TIMER_CHANNEL == LL_TIM_CHANNEL_CH1N
    LL_TIM_OC_SetCompareCH1(FURI_HAL_RFID_READ_TIMER, pulse);
#else
#error Update this code. Would you kindly?
#endif
}




static void furi_hal_rfid_rtf_carrier_out_dma_isr(void* dma_context) {
    if(LL_DMA_IsActiveFlag_HT1(DMA1)) {
        LL_DMA_ClearFlag_HT1(DMA1);
        furi_hal_rfid->dma_callback(true, dma_context);
    }

    if(LL_DMA_IsActiveFlag_TC1(DMA1)) {
        LL_DMA_ClearFlag_TC1(DMA1);
        furi_hal_rfid->dma_callback(false, dma_context);
    }
}

void furi_hal_rfid_rtf_carrier_out_start(
    uint32_t* duration,
    uint32_t* pulse,
    size_t length,
    FuriHalRfidDMACallback callback,
    void* context) {
    furi_assert(furi_hal_rfid);

    // setup interrupts
    furi_hal_rfid->dma_callback = callback;
	
    // configure timer
    furi_hal_bus_enable(CARRIER_OUT_TIMER_BUS);

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = SystemCoreClock / (125000 * 8) -
                               1; // sets the basis TIMER frequency to 8*freq (1MHz) for the timer
    TIM_InitStruct.Autoreload = 8 - 1; //initial PWM period =125kHz
    LL_TIM_Init(CARRIER_OUT_TIMER, &TIM_InitStruct);
    LL_TIM_DisableARRPreload(CARRIER_OUT_TIMER);

    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_ENABLE;
    TIM_OC_InitStruct.CompareValue = 8 / 2; //initial pulse duration of half period
    LL_TIM_OC_Init(CARRIER_OUT_TIMER, CARRIER_OUT_TIMER_CHANNEL, &TIM_OC_InitStruct);

    LL_TIM_OC_SetPolarity(CARRIER_OUT_TIMER, CARRIER_OUT_TIMER_CHANNEL, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_EnableDMAReq_UPDATE(CARRIER_OUT_TIMER);

    // configure DMA "mem -> ARR" channel
    LL_DMA_InitTypeDef dma_config = {0};
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (CARRIER_OUT_TIMER->ARR);
    dma_config.MemoryOrM2MDstAddress = (uint32_t)duration;
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.NbData = length;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM1_UP;
    dma_config.Priority = LL_DMA_MODE_NORMAL;
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &dma_config);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);

    // configure DMA "mem -> CCR1" channel
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (CARRIER_OUT_TIMER->CCR1);
    dma_config.MemoryOrM2MDstAddress = (uint32_t)pulse;
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.NbData = length;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM1_UP;
    dma_config.Priority = LL_DMA_MODE_NORMAL;
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_2, &dma_config);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);

    // attach interrupt to one of DMA channels
    furi_hal_interrupt_set_isr(
        FuriHalInterruptIdDma1Ch1, furi_hal_rfid_rtf_carrier_out_dma_isr, context);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_1);

    // start
    LL_TIM_EnableAllOutputs(CARRIER_OUT_TIMER);

    LL_TIM_SetCounter(CARRIER_OUT_TIMER, 0);
    LL_TIM_EnableCounter(CARRIER_OUT_TIMER);
}

void furi_hal_rfid_rtf_carrier_out_stop() {
    LL_TIM_DisableCounter(CARRIER_OUT_TIMER);
    LL_TIM_DisableAllOutputs(CARRIER_OUT_TIMER);

    furi_hal_interrupt_set_isr(FuriHalInterruptIdDma1Ch1, NULL, NULL);
    LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_DisableIT_HT(DMA1, LL_DMA_CHANNEL_1);

    FURI_CRITICAL_ENTER();

    LL_DMA_DeInit(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_DeInit(DMA1, LL_DMA_CHANNEL_2);

    furi_hal_bus_disable(CARRIER_OUT_TIMER_BUS);

    FURI_CRITICAL_EXIT();
}

static void furi_hal_rfid_rtf_carrier_in_IC_mode();

static void furi_hal_rfid_rtf_carrier_in_isr(void* capture_context) {
    uint32_t TIMval = 0;
	//note that I'm doing 2 captures during ETR mode (pulse & duration) and sending both towards hitagworker as a duration, though this is only used for logging, no processing is done
	//while in IC mode I'm only doing 1 capture (duration only) and sending this towards hitagworker as a duration used for command detection
	if(LL_TIM_IsActiveFlag_CC1(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER)) {
        //INPUT CAPTURE trigger on channel 1 for CARRIER_IN_TIMER (TIM2) is used in command detection mode to capture duration
        uint32_t newTIMval = LL_TIM_IC_GetCaptureCH1(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
        LL_TIM_ClearFlag_CC1(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);

        //ARR for TIM2 in input capture mode is UINT32_MAX so at freq of 1MHz this is 70+ minutes, instead of resetting counter, just store prev value and take difference of both
        //this increases measurement accuracy, since value is then purely hardware controlled, no dependency of interrupt timing to (re)set any timer values
        TIMval = newTIMval - furi_hal_rfid->prevTIMval;
        furi_hal_rfid->prevTIMval = newTIMval;
    } else if(LL_TIM_IsActiveFlag_UPDATE(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER)) {
        //UPDATE is used in reply mode (ETR +DMA setup) to capture duration (for logging/debugging only)
        TIMval = LL_TIM_GetCounter(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER);
        LL_TIM_ClearFlag_UPDATE(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);

        LL_TIM_SetCounter(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER, 0);
    } else if(LL_TIM_IsActiveFlag_CC3(FURI_HAL_RFID_RTF_PULL_OUT_TIMER)) {
        //OUTPUT COMPARE trigger on channel 3 for PULL_OUT_TIMER (TIM2) is used in reply mode (ETR + DMA setup) to also capture pulse (for logging/debugging only)
        TIMval = LL_TIM_GetCounter(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER);
        LL_TIM_ClearFlag_CC3(FURI_HAL_RFID_RTF_PULL_OUT_TIMER);
    }
	
	if (TIMval != 0){
        furi_hal_rfid->read_capture_callback(true, TIMval, capture_context);
    }
}

static void furi_hal_rfid_rtf_pull_out_dma_stop() {
    //reconfigure pull_out pin to fixed low state
    //via forced OC INACTIVE MODE
    LL_TIM_OC_SetMode(FURI_HAL_RFID_RTF_PULL_OUT_TIMER, FURI_HAL_RFID_RTF_PULL_OUT_TIMER_CHANNEL, LL_TIM_OCMODE_FORCED_INACTIVE);

    //disable DMA channels & requests
    LL_TIM_DisableDMAReq_UPDATE(FURI_HAL_RFID_RTF_PULL_OUT_TIMER);
    LL_DMA_DisableChannel(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1_DEF); //need to disable when using normal mode?
    LL_DMA_DisableChannel(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH2_DEF); //need to disable when using normal mode?
    //LL_TIM_DisableAllOutputs(FURI_HAL_RFID_RTF_PULL_OUT_TIMER); 		//no need to disable when reconfiguring pin?

    //for logging both ccr & arr times during emulate (iso only arr)
    LL_TIM_DisableIT_CC3(FURI_HAL_RFID_RTF_PULL_OUT_TIMER);

    //switch carrier detection back to input capture for stable readings (required for command detection)
    furi_hal_rfid_rtf_carrier_in_IC_mode();
}

static void furi_hal_rfid_rtf_pull_out_dma_isr() {
    // currently no HT interrupt enabled, only TC
	if(LL_DMA_IsActiveFlag_TC1(FURI_HAL_RFID_RTF_PULL_OUT_DMA)) {
        LL_DMA_ClearFlag_TC1(FURI_HAL_RFID_RTF_PULL_OUT_DMA);
        furi_hal_rfid_rtf_pull_out_dma_stop();
    }
}

void furi_hal_rfid_rtf_pull_out_dma_start(size_t length) {
    //array pointers remain the same, but length changes so
    //reset DMA length (only possible when DMA channel is disabled
    //this also resets the DMA counter (the DMA length is the 'remaining counter'
    LL_DMA_SetDataLength(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1_DEF, length);
    LL_DMA_SetDataLength(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH2_DEF, length);

    //enable DMA
    LL_DMA_EnableChannel(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1_DEF);
    LL_DMA_EnableChannel(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH2_DEF);

    //set OC to PWM1 mode
    LL_TIM_OC_SetMode(
        FURI_HAL_RFID_RTF_PULL_OUT_TIMER,
        FURI_HAL_RFID_RTF_PULL_OUT_TIMER_CHANNEL,
        LL_TIM_OCMODE_PWM1); //during reply (dma controlled) mode

    //only enable DMA requests on timer update after enabling both channels (otherwise you risk that first DMA request only triggers one of the channels)
    LL_TIM_EnableDMAReq_UPDATE(FURI_HAL_RFID_RTF_PULL_OUT_TIMER);

    //for logging both ccr & arr times during emulate (iso only arr)
    LL_TIM_EnableIT_CC3(FURI_HAL_RFID_RTF_PULL_OUT_TIMER);
}

static void furi_hal_rfid_rtf_pull_out_dma_setup(
    uint32_t* duration,
    uint32_t* pulse,
    size_t length) {
    //DMA setup
    LL_TIM_DisableDMAReq_UPDATE(FURI_HAL_RFID_RTF_PULL_OUT_TIMER); //start with DMA requests disabled

    // configure DMA "mem -> ARR" channel
    LL_DMA_InitTypeDef dma_config = {0};
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (FURI_HAL_RFID_RTF_PULL_OUT_TIMER->ARR);
    dma_config.MemoryOrM2MDstAddress = (uint32_t)duration;
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    //dma_config.Mode = LL_DMA_MODE_CIRCULAR;	//keep cycling through memory array
    dma_config.Mode = LL_DMA_MODE_NORMAL; //cycle only once through memory array
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.NbData = length;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    dma_config.Priority = LL_DMA_MODE_NORMAL;
    LL_DMA_Init(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1_DEF, &dma_config);

    // configure DMA "mem -> CCR3" channel
    dma_config.PeriphOrM2MSrcAddress = (uint32_t) & (FURI_HAL_RFID_RTF_PULL_OUT_TIMER->CCR3);
    dma_config.MemoryOrM2MDstAddress = (uint32_t)pulse;
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    //dma_config.Mode = LL_DMA_MODE_CIRCULAR;	//keep cycling through memory array
    dma_config.Mode = LL_DMA_MODE_NORMAL; //cycle only once through memory array
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.NbData = length;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM2_UP;
    dma_config.Priority = LL_DMA_MODE_NORMAL;
    LL_DMA_Init(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH2_DEF, &dma_config);

    // enable DMA interrupts
    furi_hal_interrupt_set_isr(
        FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1_IRQ, furi_hal_rfid_rtf_pull_out_dma_isr, NULL);

    //LL_DMA_EnableIT_HT(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1_DEF);	//let's try normal DMA mode and only transfer complete interrupt
    LL_DMA_EnableIT_TC(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1_DEF);
}

void furi_hal_rfid_rtf_carrier_in_ETR_mode(uint8_t ext_prescaler) {
    //ETR mode used during emulation while replying to commands from reader
    //it has less accuracy than IC mode, but microcontroller cannot run DMA for pull out and input capture for carrier in at the same time
	
    //disable counters temporarily
    LL_TIM_DisableCounter(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
    LL_TIM_DisableCounter(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER);

    //disable Input capture & related interrupt
    LL_TIM_DisableIT_CC1(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
    LL_TIM_CC_DisableChannel(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_DIR_CH);

    //reset previous TIMER value
    furi_hal_rfid->prevTIMval = 0;

    //switch clocksource to ETR with external prescaling via ARR
    LL_TIM_SetPrescaler(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, 1 - 1); //prescaler is only applied at next update event
    LL_TIM_GenerateEvent_UPDATE(
        FURI_HAL_RFID_RTF_CARRIER_IN_TIMER); //prescaler is only applied at next update event //TODO: how to prevent this from creating a capture data entry
    LL_TIM_SetAutoReload(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, ext_prescaler - 1);
    LL_TIM_SetClockSource(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, LL_TIM_CLOCKSOURCE_EXT_MODE2);

    //reconfigure carrier_in pin to TIM2 ETR
    furi_hal_gpio_init_ex(
        &gpio_rfid_carrier, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn2TIM2);

    //reset timer counter & capture context for period calculation
    LL_TIM_SetCounter(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, 0);
    LL_TIM_SetCounter(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER, 0);

    //enable interrupt via update
    LL_TIM_EnableIT_UPDATE(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);

    //re-enable counters
    LL_TIM_EnableCounter(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
    LL_TIM_EnableCounter(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER);
}

static void furi_hal_rfid_rtf_carrier_in_IC_mode() {
    //input caputre mode used during emulation while scanning for commands from reader. Input capture yields more accurate results compared to ETR.
    
    //disable counters temporarily
    LL_TIM_DisableCounter(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
    LL_TIM_DisableCounter(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER);

    //disable interrupt via update
    LL_TIM_DisableIT_UPDATE(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);

    //reset previous TIMER value
    furi_hal_rfid->prevTIMval = 0;

    //switch clocksource to system/64, external prescaling is handled in IC prescaler
    LL_TIM_SetClockSource(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_SetPrescaler(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, 64 - 1); //prescaler is only applied at next update event
    LL_TIM_GenerateEvent_UPDATE(
        FURI_HAL_RFID_RTF_CARRIER_IN_TIMER); //prescaler is only applied at next update event	//TODO: how to prevent this from creating a capture data entry
    LL_TIM_SetAutoReload(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, UINT32_MAX);

    //reconfigure carrier_in pin to TIM2 CH1 for input capture
    furi_hal_gpio_init_ex(
        &gpio_rfid_carrier, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn1TIM2);

    //reset timer counter & capture context for period calculation
    LL_TIM_SetCounter(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, 0);
    LL_TIM_SetCounter(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER, 0);

    //enable Input capture & related interrupt
    LL_TIM_EnableIT_CC1(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
    LL_TIM_CC_EnableChannel(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_DIR_CH);

    //re-enable counters
    LL_TIM_EnableCounter(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
    LL_TIM_EnableCounter(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER);
}

void furi_hal_rfid_rtf_carrier_in_start(
    void* capture_context,
    uint8_t ext_prescaler,
    uint32_t* duration,
    uint32_t* pulse,
    size_t length,
    FuriHalRfidReadCaptureCallback callback) {
    furi_assert(furi_hal_rfid);
    furi_hal_rfid->read_capture_callback = callback;

    FURI_CRITICAL_ENTER();
    LL_DMA_DeInit(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1_DEF); //required?
    LL_DMA_DeInit(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH2_DEF); //required?
    FURI_CRITICAL_EXIT();

    furi_hal_bus_enable(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_BUS);
    furi_hal_bus_enable(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER_BUS);

    //setup reference timer: simple setup with base freq of 1MHz and max autoreload
    LL_TIM_InitTypeDef TIM_InitStruct_Ref = {0};
    TIM_InitStruct_Ref.Prescaler =
        64 -
        1; //system base freq is 64MHz, so this sets base freq for TIM to 1MHz (aka 1us period)
    TIM_InitStruct_Ref.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct_Ref.Autoreload = UINT32_MAX;
    TIM_InitStruct_Ref.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER, &TIM_InitStruct_Ref);

    //setup carrier in timer for input capture
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler =
        64 -
        1; //system base freq is 64MHz, so this sets base freq for TIM to 1MHz (aka 1us period)
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = UINT32_MAX;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, &TIM_InitStruct);

    LL_TIM_DisableARRPreload(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
    LL_TIM_SetClockSource(
        FURI_HAL_RFID_RTF_CARRIER_IN_TIMER,
        LL_TIM_CLOCKSOURCE_INTERNAL); //default is internal, so likely not required
    LL_TIM_DisableDMAReq_TRIG(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
    LL_TIM_DisableIT_TRIG(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);

    //meanwhile already prepare the ETR
    LL_TIM_ConfigETR(
        FURI_HAL_RFID_RTF_CARRIER_IN_TIMER,
        LL_TIM_ETR_POLARITY_INVERTED,
        LL_TIM_ETR_PRESCALER_DIV1,
        LL_TIM_ETR_FILTER_FDIV1);

    //INPUT CAPTURE SETUP
    // Timer: channel 1 direct (from GPIO)
    LL_TIM_IC_SetActiveInput(
        FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_DIR_CH, LL_TIM_ACTIVEINPUT_DIRECTTI);
    //prescaling direct channel seems to be working fine (and is necessary since otherwise sd write cannot keep up)
    if(ext_prescaler == 4) {
        LL_TIM_IC_SetPrescaler(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_DIR_CH, LL_TIM_ICPSC_DIV4);
    } else if(ext_prescaler == 2) {
        LL_TIM_IC_SetPrescaler(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_DIR_CH, LL_TIM_ICPSC_DIV2);
    } else {
        LL_TIM_IC_SetPrescaler(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_DIR_CH, LL_TIM_ICPSC_DIV1);
    }
    LL_TIM_IC_SetPolarity(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_DIR_CH, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_IC_SetFilter(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_DIR_CH, LL_TIM_IC_FILTER_FDIV1);
    
    //set interrupt callback for capturing period
    LL_TIM_EnableIT_CC1(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
    //LL_TIM_EnableIT_CC2(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
    furi_hal_interrupt_set_isr(
        FuriHalInterruptIdTIM2, furi_hal_rfid_rtf_carrier_in_isr, capture_context);

    //OUTPUT COMPARE SETUP
    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    //TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;			//during emulate (dma controlled) mode
    TIM_OC_InitStruct.OCMode =
        LL_TIM_OCMODE_FORCED_INACTIVE; //during carrier in, put output to forced low state
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
    TIM_OC_InitStruct.CompareValue =
        0; //0% this should have almost similar effect as keeping output forced inactive (there's still some micropulse emited, going high at ARR and immediately down again at CCR value)
    LL_TIM_OC_Init(FURI_HAL_RFID_RTF_PULL_OUT_TIMER, FURI_HAL_RFID_RTF_PULL_OUT_TIMER_CHANNEL, &TIM_OC_InitStruct);
    LL_TIM_OC_SetPolarity(
        FURI_HAL_RFID_RTF_PULL_OUT_TIMER,
        FURI_HAL_RFID_RTF_PULL_OUT_TIMER_CHANNEL,
        LL_TIM_OCPOLARITY_HIGH); //active high (gpio goes high when pulse is high)

    //INIT DMA (do not start it yet)
    furi_hal_rfid_rtf_pull_out_dma_setup(duration, pulse, length);

    LL_TIM_CC_EnableChannel(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_DIR_CH);
    LL_TIM_CC_EnableChannel(FURI_HAL_RFID_RTF_PULL_OUT_TIMER, FURI_HAL_RFID_RTF_PULL_OUT_TIMER_CHANNEL);
    LL_TIM_SetCounter(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER, 0);
    LL_TIM_SetCounter(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER, 0);
    LL_TIM_EnableCounter(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
    LL_TIM_EnableCounter(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER);
}

void furi_hal_rfid_rtf_carrier_in_stop() {
    furi_hal_interrupt_set_isr(FuriHalInterruptIdTIM2, NULL, NULL);
    furi_hal_interrupt_set_isr(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1_IRQ, NULL, NULL);

    LL_TIM_DisableCounter(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER);
    LL_TIM_DisableCounter(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER);
    LL_TIM_DisableAllOutputs(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER); //used for pull pin OC

    FURI_CRITICAL_ENTER();
    LL_DMA_DeInit(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH1_DEF);
    LL_DMA_DeInit(FURI_HAL_RFID_RTF_PULL_OUT_DMA_CH2_DEF);

    furi_hal_bus_disable(FURI_HAL_RFID_RTF_CARRIER_IN_TIMER_BUS);
    furi_hal_bus_disable(FURI_HAL_RFID_RTF_CARRIER_IN_REFERENCE_TIMER_BUS);

    FURI_CRITICAL_EXIT();
}





void furi_hal_rfid_comp_start() {
    LL_COMP_Enable(COMP1);
    // Magic
    uint32_t wait_loop_index = ((80 / 10UL) * ((SystemCoreClock / (100000UL * 2UL)) + 1UL));
    while(wait_loop_index) {
        wait_loop_index--;
    }
}

void furi_hal_rfid_comp_stop() {
    LL_COMP_Disable(COMP1);
}

FuriHalRfidCompCallback furi_hal_rfid_comp_callback = NULL;
void* furi_hal_rfid_comp_callback_context = NULL;

void furi_hal_rfid_comp_set_callback(FuriHalRfidCompCallback callback, void* context) {
    FURI_CRITICAL_ENTER();
    furi_hal_rfid_comp_callback = callback;
    furi_hal_rfid_comp_callback_context = context;
    __DMB();
    FURI_CRITICAL_EXIT();
}

/* Comparator trigger event */
void COMP_IRQHandler() {
    if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_20)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_20);
    }
    if(furi_hal_rfid_comp_callback) {
        furi_hal_rfid_comp_callback(
            (LL_COMP_ReadOutputLevel(COMP1) == LL_COMP_OUTPUT_LEVEL_LOW),
            furi_hal_rfid_comp_callback_context);
    }
}

static void furi_hal_rfid_field_tim_setup() {
    // setup timer counter
    furi_hal_bus_enable(FURI_HAL_RFID_FIELD_COUNTER_TIMER_BUS);

    LL_TIM_SetPrescaler(FURI_HAL_RFID_FIELD_COUNTER_TIMER, 0);
    LL_TIM_SetCounterMode(FURI_HAL_RFID_FIELD_COUNTER_TIMER, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(FURI_HAL_RFID_FIELD_COUNTER_TIMER, 0xFFFFFFFF);
    LL_TIM_DisableARRPreload(FURI_HAL_RFID_FIELD_COUNTER_TIMER);
    LL_TIM_SetRepetitionCounter(FURI_HAL_RFID_FIELD_COUNTER_TIMER, 0);

    LL_TIM_SetClockDivision(FURI_HAL_RFID_FIELD_COUNTER_TIMER, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetClockSource(FURI_HAL_RFID_FIELD_COUNTER_TIMER, LL_TIM_CLOCKSOURCE_EXT_MODE2);
    LL_TIM_ConfigETR(
        FURI_HAL_RFID_FIELD_COUNTER_TIMER,
        LL_TIM_ETR_POLARITY_INVERTED,
        LL_TIM_ETR_PRESCALER_DIV1,
        LL_TIM_ETR_FILTER_FDIV1);

    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
    TIM_OC_InitStruct.CompareValue = 1;
    LL_TIM_OC_Init(
        FURI_HAL_RFID_FIELD_COUNTER_TIMER,
        FURI_HAL_RFID_FIELD_COUNTER_TIMER_CHANNEL,
        &TIM_OC_InitStruct);

    LL_TIM_GenerateEvent_UPDATE(FURI_HAL_RFID_FIELD_COUNTER_TIMER);
    LL_TIM_OC_SetPolarity(
        FURI_HAL_RFID_FIELD_COUNTER_TIMER,
        FURI_HAL_RFID_FIELD_COUNTER_TIMER_CHANNEL,
        LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_EnableDMAReq_UPDATE(FURI_HAL_RFID_FIELD_COUNTER_TIMER);

    // setup timer timeouts dma
    furi_hal_bus_enable(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER_BUS);

    LL_TIM_SetPrescaler(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER, 64000 - 1);
    LL_TIM_SetCounterMode(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER, 100 - 1); // 100 ms
    LL_TIM_SetClockDivision(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetClockSource(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER, LL_TIM_CLOCKSOURCE_INTERNAL);

    LL_TIM_DisableARRPreload(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER);

    LL_TIM_EnableDMAReq_UPDATE(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER);
    LL_TIM_GenerateEvent_UPDATE(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER);
}

void furi_hal_rfid_field_detect_start(void) {
    // setup pins
    furi_hal_rfid_pins_field();

    // configure timer
    furi_hal_rfid_field_tim_setup();

    // configure DMA "TIM_COUNTER_CNT -> counter"
    LL_DMA_SetMemoryAddress(RFID_DMA_CH1_DEF, (uint32_t) & (furi_hal_rfid->field.counter));
    LL_DMA_SetPeriphAddress(
        RFID_DMA_CH1_DEF, (uint32_t) & (FURI_HAL_RFID_FIELD_COUNTER_TIMER->CNT));
    LL_DMA_ConfigTransfer(
        RFID_DMA_CH1_DEF,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_NOINCREMENT | LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD |
            LL_DMA_PRIORITY_MEDIUM);
    LL_DMA_SetDataLength(RFID_DMA_CH1_DEF, 1);
    LL_DMA_SetPeriphRequest(RFID_DMA_CH1_DEF, FURI_HAL_RFID_FIELD_DMAMUX_DMA);
    LL_DMA_EnableChannel(RFID_DMA_CH1_DEF);

    // configure DMA "mem -> TIM_COUNTER_CNT"
    LL_DMA_SetMemoryAddress(
        RFID_DMA_CH2_DEF, (uint32_t) & (furi_hal_rfid->field.set_tim_counter_cnt));
    LL_DMA_SetPeriphAddress(
        RFID_DMA_CH2_DEF, (uint32_t) & (FURI_HAL_RFID_FIELD_COUNTER_TIMER->CNT));
    LL_DMA_ConfigTransfer(
        RFID_DMA_CH2_DEF,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_NOINCREMENT | LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD |
            LL_DMA_PRIORITY_LOW);
    LL_DMA_SetDataLength(RFID_DMA_CH2_DEF, 1);
    LL_DMA_SetPeriphRequest(RFID_DMA_CH2_DEF, FURI_HAL_RFID_FIELD_DMAMUX_DMA);
    LL_DMA_EnableChannel(RFID_DMA_CH2_DEF);

    // start tim counter
    LL_TIM_EnableAllOutputs(FURI_HAL_RFID_FIELD_COUNTER_TIMER);

    LL_TIM_SetCounter(FURI_HAL_RFID_FIELD_COUNTER_TIMER, 0);
    LL_TIM_EnableCounter(FURI_HAL_RFID_FIELD_COUNTER_TIMER);

    // start tim timeout
    LL_TIM_SetCounter(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER, 0);
    LL_TIM_EnableCounter(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER);
    LL_TIM_EnableIT_UPDATE(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER);
}

void furi_hal_rfid_field_detect_stop(void) {
    LL_TIM_DisableCounter(FURI_HAL_RFID_FIELD_COUNTER_TIMER);
    LL_TIM_DisableAllOutputs(FURI_HAL_RFID_FIELD_COUNTER_TIMER);

    LL_TIM_DisableCounter(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER);

    FURI_CRITICAL_ENTER();

    LL_DMA_DeInit(RFID_DMA_CH1_DEF);
    LL_DMA_DeInit(RFID_DMA_CH2_DEF);

    furi_hal_bus_disable(FURI_HAL_RFID_FIELD_COUNTER_TIMER_BUS);
    furi_hal_bus_disable(FURI_HAL_RFID_FIELD_TIMEOUT_TIMER_BUS);

    furi_hal_rfid_pins_reset();

    FURI_CRITICAL_EXIT();
}

bool furi_hal_rfid_field_is_present(uint32_t* frequency) {
    *frequency = furi_hal_rfid->field.counter * 10;
    return (
        (*frequency >= FURI_HAL_RFID_FIELD_FREQUENCY_MIN) &&
        (*frequency <= FURI_HAL_RFID_FIELD_FREQUENCY_MAX));
}