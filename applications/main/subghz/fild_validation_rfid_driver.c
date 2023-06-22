#include "fild_validation_rfid_driver.h"
#include <furi_hal_ibutton.h>
#include <furi_hal_resources.h>
#include <furi_hal_bus.h>
#include <furi.h>

#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_dma.h>

#define FILD_VALIDATION_RFID_FREQUENCY_MIN 80000
#define FILD_VALIDATION_RFID_FREQUENCY_MAX 200000

#define FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER TIM2
#define FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER_BUS FuriHalBusTIM2
#define FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER_IRQ FuriHalInterruptIdTIM2
#define FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER_CHANNEL LL_TIM_CHANNEL_CH3

#define FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT TIM17
#define FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT_BUS FuriHalBusTIM17
#define FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT_IRQ FuriHalInterruptIdTim1TrgComTim17

/* DMA Channels definition */
#define FILD_VALIDATION_RFID_DMA DMA2
#define FILD_VALIDATION_RFID_DMA_CH1_CHANNEL LL_DMA_CHANNEL_1
#define FILD_VALIDATION_RFID_DMA_CH2_CHANNEL LL_DMA_CHANNEL_2
#define FILD_VALIDATION_RFID_DMA_CH1_IRQ FuriHalInterruptIdDma2Ch1
#define FILD_VALIDATION_RFID_DMA_CH1_DEF \
    FILD_VALIDATION_RFID_DMA, FILD_VALIDATION_RFID_DMA_CH1_CHANNEL
#define FILD_VALIDATION_RFID_DMA_CH2_DEF \
    FILD_VALIDATION_RFID_DMA, FILD_VALIDATION_RFID_DMA_CH2_CHANNEL

struct FildValdationDriverRfid {
    uint32_t counter;
    uint32_t set_tim_counter_cnt;
};

FildValdationDriverRfid* fild_validation_rfid_driver_alloc() {
    FildValdationDriverRfid* instance = malloc(sizeof(FildValdationDriverRfid));
    instance->counter = 0;
    instance->set_tim_counter_cnt = 0;
    return instance;
}

void fild_validation_rfid_driver_free(FildValdationDriverRfid* instance) {
    furi_assert(instance);
    free(instance);
}

bool fild_validation_rfid_driver_check(FildValdationDriverRfid* instance, uint32_t* frequency) {
    furi_assert(instance);
    *frequency = instance->counter * 10;
    return (
        (*frequency >= FILD_VALIDATION_RFID_FREQUENCY_MIN) &&
        (*frequency <= FILD_VALIDATION_RFID_FREQUENCY_MAX));
}

static void fild_validation_rfid_driver_pins_reset() {
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

static void fild_validation_rfid_driver_pins_setup() {
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

static void fild_validation_rfid_driver_tim_setup() {
    // setup timer counter
    furi_hal_bus_enable(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER_BUS);

    LL_TIM_SetPrescaler(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER, 0);
    LL_TIM_SetCounterMode(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER, 0xFFFFFFFF);
    LL_TIM_DisableARRPreload(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER);
    LL_TIM_SetRepetitionCounter(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER, 0);

    LL_TIM_SetClockDivision(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetClockSource(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER, LL_TIM_CLOCKSOURCE_EXT_MODE2);
    LL_TIM_ConfigETR(
        FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER,
        LL_TIM_ETR_POLARITY_INVERTED,
        LL_TIM_ETR_PRESCALER_DIV1,
        LL_TIM_ETR_FILTER_FDIV1);

    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
    TIM_OC_InitStruct.CompareValue = 1;
    LL_TIM_OC_Init(
        FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER,
        FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER_CHANNEL,
        &TIM_OC_InitStruct);

    LL_TIM_GenerateEvent_UPDATE(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER);
    LL_TIM_OC_SetPolarity(
        FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER,
        FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER_CHANNEL,
        LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_EnableDMAReq_UPDATE(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER);

    // setup timer timeouts dma
    furi_hal_bus_enable(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT_BUS);

    LL_TIM_SetPrescaler(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT, 64000 - 1);
    LL_TIM_SetCounterMode(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT, 100); // 100 ms
    LL_TIM_SetClockDivision(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetClockSource(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT, LL_TIM_CLOCKSOURCE_INTERNAL);

    LL_TIM_DisableARRPreload(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT);

    LL_TIM_EnableDMAReq_UPDATE(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT);
    LL_TIM_GenerateEvent_UPDATE(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT);
}

void fild_validation_rfid_driver_dma_start(FildValdationDriverRfid* instance) {
    furi_assert(instance);
    UNUSED(instance);

    // setup pins
    fild_validation_rfid_driver_pins_setup();

    // configure timer
    fild_validation_rfid_driver_tim_setup();

    // configure DMA "TIM_COUNTER_CNT -> counter"
    LL_DMA_SetMemoryAddress(FILD_VALIDATION_RFID_DMA_CH1_DEF, (uint32_t) & (instance->counter));
    LL_DMA_SetPeriphAddress(
        FILD_VALIDATION_RFID_DMA_CH1_DEF,
        (uint32_t) & (FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER->CNT));
    LL_DMA_ConfigTransfer(
        FILD_VALIDATION_RFID_DMA_CH1_DEF,
        LL_DMA_DIRECTION_PERIPH_TO_MEMORY | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_NOINCREMENT | LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD |
            LL_DMA_PRIORITY_MEDIUM);
    LL_DMA_SetDataLength(FILD_VALIDATION_RFID_DMA_CH1_DEF, 1);
    LL_DMA_SetPeriphRequest(FILD_VALIDATION_RFID_DMA_CH1_DEF, LL_DMAMUX_REQ_TIM17_UP);
    LL_DMA_EnableChannel(FILD_VALIDATION_RFID_DMA_CH1_DEF);

    // configure DMA "mem -> TIM_COUNTER_CNT"
    LL_DMA_SetMemoryAddress(
        FILD_VALIDATION_RFID_DMA_CH2_DEF, (uint32_t) & (instance->set_tim_counter_cnt));
    LL_DMA_SetPeriphAddress(
        FILD_VALIDATION_RFID_DMA_CH2_DEF,
        (uint32_t) & (FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER->CNT));
    LL_DMA_ConfigTransfer(
        FILD_VALIDATION_RFID_DMA_CH2_DEF,
        LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_MODE_CIRCULAR | LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_NOINCREMENT | LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD |
            LL_DMA_PRIORITY_LOW);
    LL_DMA_SetDataLength(FILD_VALIDATION_RFID_DMA_CH2_DEF, 1);
    LL_DMA_SetPeriphRequest(FILD_VALIDATION_RFID_DMA_CH2_DEF, LL_DMAMUX_REQ_TIM17_UP);
    LL_DMA_EnableChannel(FILD_VALIDATION_RFID_DMA_CH2_DEF);

    // start tim counter
    LL_TIM_EnableAllOutputs(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER);

    LL_TIM_SetCounter(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER, 0);
    LL_TIM_EnableCounter(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER);

    // start tim timeout
    LL_TIM_SetCounter(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT, 0);
    LL_TIM_EnableCounter(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT);
    LL_TIM_EnableIT_UPDATE(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT);
}

void fild_validation_rfid_driver_dma_stop(FildValdationDriverRfid* instance) {
    furi_assert(instance);
    UNUSED(instance);

    LL_TIM_DisableCounter(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER);
    LL_TIM_DisableAllOutputs(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER);

    LL_TIM_DisableCounter(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT);

    FURI_CRITICAL_ENTER();

    LL_DMA_DeInit(FILD_VALIDATION_RFID_DMA_CH1_DEF);
    LL_DMA_DeInit(FILD_VALIDATION_RFID_DMA_CH2_DEF);

    furi_hal_bus_disable(FILD_VALIDATION_RFID_DRIVER_TIMER_COUNTER_BUS);
    furi_hal_bus_disable(FILD_VALIDATION_RFID_DRIVER_TIMER_TIMEOUT_BUS);

    fild_validation_rfid_driver_pins_reset();

    FURI_CRITICAL_EXIT();
}
