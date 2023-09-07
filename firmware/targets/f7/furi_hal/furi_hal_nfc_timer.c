#include "furi_hal_nfc_i.h"

#include <furi_hal_interrupt.h>
#include <stm32wbxx_ll_tim.h>
#include <furi_hal_resources.h>
#include <furi_hal_gpio.h>
#include <furi_hal_bus.h>

#define FURI_HAL_NFC_FREQ_KHZ (13560U)

typedef enum {
    FuriHalNfcTimerFwt,
    FuriHalNfcTimerBlockTx,
    FuriHalNfcTimerCount,
} FuriHalNfcTimer;

typedef struct {
    TIM_TypeDef* timer;
    FuriHalBus bus;
    uint32_t prescaler;
    uint32_t freq_khz;
    FuriHalNfcEventInternalType event;
    FuriHalInterruptId irq_id;
    IRQn_Type irq_type;
    bool is_configured;
    const GpioPin* pin;
} FuriHalNfcTimerConfig;

static FuriHalNfcTimerConfig furi_hal_nfc_timers[FuriHalNfcTimerCount] = {
    [FuriHalNfcTimerFwt] =
        {
            .pin = &gpio_ext_pa7,
            .timer = TIM1,
            .bus = FuriHalBusTIM1,
            .prescaler = 15,
            .freq_khz = 4000U,
            .event = FuriHalNfcEventInternalTypeTimerFwtExpired,
            .irq_id = FuriHalInterruptIdTim1UpTim16,
            .irq_type = TIM1_UP_TIM16_IRQn,
            .is_configured = false,
        },
    [FuriHalNfcTimerBlockTx] =
        {
            .pin = &gpio_ext_pa6,
            .timer = TIM17,
            .bus = FuriHalBusTIM17,
            .prescaler = 31,
            .freq_khz = 2000U,
            .event = FuriHalNfcEventInternalTypeTimerBlockTxExpired,
            .irq_id = FuriHalInterruptIdTim1TrgComTim17,
            .irq_type = TIM1_TRG_COM_TIM17_IRQn,
            .is_configured = false,
        },
};

static void furi_hal_nfc_timer_irq_callback(void* context) {
    FuriHalNfcTimerConfig* timer = context;
    if(LL_TIM_IsActiveFlag_UPDATE(timer->timer)) {
        LL_TIM_ClearFlag_UPDATE(timer->timer);
        furi_hal_nfc_event_set(timer->event);
        furi_hal_gpio_write(timer->pin, false);
    }
}

static void furi_hal_nfc_timer_init(FuriHalNfcTimer timer) {
    furi_hal_bus_enable(furi_hal_nfc_timers[timer].bus);

    LL_TIM_SetPrescaler(furi_hal_nfc_timers[timer].timer, furi_hal_nfc_timers[timer].prescaler);
    LL_TIM_SetOnePulseMode(furi_hal_nfc_timers[timer].timer, LL_TIM_ONEPULSEMODE_SINGLE);
    LL_TIM_EnableUpdateEvent(furi_hal_nfc_timers[timer].timer);
    LL_TIM_SetCounterMode(furi_hal_nfc_timers[timer].timer, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockSource(furi_hal_nfc_timers[timer].timer, LL_TIM_CLOCKSOURCE_INTERNAL);

    LL_TIM_GenerateEvent_UPDATE(furi_hal_nfc_timers[timer].timer);

    LL_TIM_EnableIT_UPDATE(furi_hal_nfc_timers[timer].timer);

    furi_hal_interrupt_set_isr(
        furi_hal_nfc_timers[timer].irq_id,
        furi_hal_nfc_timer_irq_callback,
        &furi_hal_nfc_timers[timer]);
    NVIC_SetPriority(
        furi_hal_nfc_timers[timer].irq_type,
        NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(furi_hal_nfc_timers[timer].irq_type);
    furi_hal_nfc_timers[timer].is_configured = true;
    furi_hal_gpio_init(
        furi_hal_nfc_timers[timer].pin, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(furi_hal_nfc_timers[timer].pin, false);
}

static void furi_hal_nfc_timer_deinit(FuriHalNfcTimer timer) {
    LL_TIM_ClearFlag_UPDATE(furi_hal_nfc_timers[timer].timer);
    furi_hal_interrupt_set_isr(furi_hal_nfc_timers[timer].irq_id, NULL, NULL);
    NVIC_DisableIRQ(furi_hal_nfc_timers[timer].irq_type);
    furi_hal_nfc_timers[timer].is_configured = false;

    if(furi_hal_bus_is_enabled(furi_hal_nfc_timers[timer].bus)) {
        furi_hal_bus_disable(furi_hal_nfc_timers[timer].bus);
    }
}

static void furi_hal_nfc_timer_start(FuriHalNfcTimer timer, uint32_t time_fc) {
    uint32_t arr_reg = furi_hal_nfc_timers[timer].freq_khz * time_fc / FURI_HAL_NFC_FREQ_KHZ;
    furi_check(arr_reg < UINT16_MAX);

    LL_TIM_SetAutoReload(furi_hal_nfc_timers[timer].timer, arr_reg);
    LL_TIM_EnableCounter(furi_hal_nfc_timers[timer].timer);
    furi_hal_gpio_write(furi_hal_nfc_timers[timer].pin, true);
}

static void furi_hal_nfc_timer_stop(FuriHalNfcTimer timer) {
    LL_TIM_DisableCounter(furi_hal_nfc_timers[timer].timer);
    LL_TIM_SetCounter(furi_hal_nfc_timers[timer].timer, 0);
    LL_TIM_SetAutoReload(furi_hal_nfc_timers[timer].timer, 0);
    if(LL_TIM_IsActiveFlag_UPDATE(furi_hal_nfc_timers[timer].timer)) {
        LL_TIM_ClearFlag_UPDATE(furi_hal_nfc_timers[timer].timer);
    }
    furi_hal_gpio_write(furi_hal_nfc_timers[timer].pin, false);
}

void furi_hal_nfc_timers_init() {
    for(size_t i = 0; i < FuriHalNfcTimerCount; i++) {
        furi_hal_nfc_timer_init(i);
    }
}

void furi_hal_nfc_timers_deinit() {
    for(size_t i = 0; i < FuriHalNfcTimerCount; i++) {
        furi_hal_nfc_timer_deinit(i);
    }
}

void furi_hal_nfc_timer_fwt_start(uint32_t time_fc) {
    furi_check(furi_hal_nfc_timers[FuriHalNfcTimerFwt].is_configured);
    furi_hal_nfc_timer_start(FuriHalNfcTimerFwt, time_fc);
}

void furi_hal_nfc_timer_fwt_stop() {
    furi_check(furi_hal_nfc_timers[FuriHalNfcTimerFwt].is_configured);
    furi_hal_nfc_timer_stop(FuriHalNfcTimerFwt);
}

void furi_hal_nfc_timer_block_tx_start(uint32_t time_fc) {
    furi_check(furi_hal_nfc_timers[FuriHalNfcTimerBlockTx].is_configured);
    furi_check(!furi_hal_nfc_timer_block_tx_is_running());

    furi_hal_nfc_timer_start(FuriHalNfcTimerBlockTx, time_fc);
}

void furi_hal_nfc_timer_block_tx_start_us(uint32_t time_us) {
    furi_check(furi_hal_nfc_timers[FuriHalNfcTimerBlockTx].is_configured);
    furi_check(!furi_hal_nfc_timer_block_tx_is_running());

    uint32_t arr_reg = furi_hal_nfc_timers[FuriHalNfcTimerBlockTx].freq_khz / 1000 * time_us;
    furi_check(arr_reg < UINT16_MAX);

    LL_TIM_SetAutoReload(furi_hal_nfc_timers[FuriHalNfcTimerBlockTx].timer, arr_reg);
    LL_TIM_EnableCounter(furi_hal_nfc_timers[FuriHalNfcTimerBlockTx].timer);
    furi_hal_gpio_write(furi_hal_nfc_timers[FuriHalNfcTimerBlockTx].pin, true);
}

void furi_hal_nfc_timer_block_tx_stop() {
    furi_check(furi_hal_nfc_timers[FuriHalNfcTimerBlockTx].is_configured);
    furi_hal_nfc_timer_stop(FuriHalNfcTimerBlockTx);
}

bool furi_hal_nfc_timer_block_tx_is_running() {
    return LL_TIM_IsEnabledCounter(furi_hal_nfc_timers[FuriHalNfcTimerBlockTx].timer) == 1;
}
