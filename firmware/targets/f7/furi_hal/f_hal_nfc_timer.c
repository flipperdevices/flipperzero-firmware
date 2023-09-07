#include "f_hal_nfc_i.h"

#include <furi_hal_interrupt.h>
#include <stm32wbxx_ll_tim.h>
#include <furi_hal_resources.h>
#include <furi_hal_gpio.h>
#include <furi_hal_bus.h>

#define F_HAL_NFC_FREQ_KHZ (13560U)

typedef enum {
    FHalNfcTimerFwt,
    FHalNfcTimerBlockTx,
    FHalNfcTimerCount,
} FHalNfcTimer;

typedef struct {
    TIM_TypeDef* timer;
    FuriHalBus bus;
    uint32_t prescaler;
    uint32_t freq_khz;
    FHalNfcEventInternalType event;
    FuriHalInterruptId irq_id;
    IRQn_Type irq_type;
    bool is_configured;
    const GpioPin* pin;
} FHalNfcTimerConfig;

static FHalNfcTimerConfig f_hal_nfc_timers[FHalNfcTimerCount] = {
    [FHalNfcTimerFwt] =
        {
            .pin = &gpio_ext_pa7,
            .timer = TIM1,
            .bus = FuriHalBusTIM1,
            .prescaler = 15,
            .freq_khz = 4000U,
            .event = FHalNfcEventInternalTypeTimerFwtExpired,
            .irq_id = FuriHalInterruptIdTim1UpTim16,
            .irq_type = TIM1_UP_TIM16_IRQn,
            .is_configured = false,
        },
    [FHalNfcTimerBlockTx] =
        {
            .pin = &gpio_ext_pa6,
            .timer = TIM17,
            .bus = FuriHalBusTIM17,
            .prescaler = 31,
            .freq_khz = 2000U,
            .event = FHalNfcEventInternalTypeTimerBlockTxExpired,
            .irq_id = FuriHalInterruptIdTim1TrgComTim17,
            .irq_type = TIM1_TRG_COM_TIM17_IRQn,
            .is_configured = false,
        },
};

static void f_hal_nfc_timer_irq_callback(void* context) {
    FHalNfcTimerConfig* timer = context;
    if(LL_TIM_IsActiveFlag_UPDATE(timer->timer)) {
        LL_TIM_ClearFlag_UPDATE(timer->timer);
        f_hal_nfc_event_set(timer->event);
        furi_hal_gpio_write(timer->pin, false);
    }
}

static void f_hal_nfc_timer_init(FHalNfcTimer timer) {
    furi_hal_bus_enable(f_hal_nfc_timers[timer].bus);

    LL_TIM_SetPrescaler(f_hal_nfc_timers[timer].timer, f_hal_nfc_timers[timer].prescaler);
    LL_TIM_SetOnePulseMode(f_hal_nfc_timers[timer].timer, LL_TIM_ONEPULSEMODE_SINGLE);
    LL_TIM_EnableUpdateEvent(f_hal_nfc_timers[timer].timer);
    LL_TIM_SetCounterMode(f_hal_nfc_timers[timer].timer, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockSource(f_hal_nfc_timers[timer].timer, LL_TIM_CLOCKSOURCE_INTERNAL);

    LL_TIM_GenerateEvent_UPDATE(f_hal_nfc_timers[timer].timer);

    LL_TIM_EnableIT_UPDATE(f_hal_nfc_timers[timer].timer);

    furi_hal_interrupt_set_isr(
        f_hal_nfc_timers[timer].irq_id, f_hal_nfc_timer_irq_callback, &f_hal_nfc_timers[timer]);
    NVIC_SetPriority(
        f_hal_nfc_timers[timer].irq_type, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(f_hal_nfc_timers[timer].irq_type);
    f_hal_nfc_timers[timer].is_configured = true;
    furi_hal_gpio_init(
        f_hal_nfc_timers[timer].pin, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(f_hal_nfc_timers[timer].pin, false);
}

static void f_hal_nfc_timer_deinit(FHalNfcTimer timer) {
    LL_TIM_ClearFlag_UPDATE(f_hal_nfc_timers[timer].timer);
    furi_hal_interrupt_set_isr(f_hal_nfc_timers[timer].irq_id, NULL, NULL);
    NVIC_DisableIRQ(f_hal_nfc_timers[timer].irq_type);
    f_hal_nfc_timers[timer].is_configured = false;

    if(furi_hal_bus_is_enabled(f_hal_nfc_timers[timer].bus)) {
        furi_hal_bus_disable(f_hal_nfc_timers[timer].bus);
    }
}

static void f_hal_nfc_timer_start(FHalNfcTimer timer, uint32_t time_fc) {
    uint32_t arr_reg = f_hal_nfc_timers[timer].freq_khz * time_fc / F_HAL_NFC_FREQ_KHZ;
    furi_check(arr_reg < UINT16_MAX);

    LL_TIM_SetAutoReload(f_hal_nfc_timers[timer].timer, arr_reg);
    LL_TIM_EnableCounter(f_hal_nfc_timers[timer].timer);
    furi_hal_gpio_write(f_hal_nfc_timers[timer].pin, true);
}

static void f_hal_nfc_timer_stop(FHalNfcTimer timer) {
    LL_TIM_DisableCounter(f_hal_nfc_timers[timer].timer);
    LL_TIM_SetCounter(f_hal_nfc_timers[timer].timer, 0);
    LL_TIM_SetAutoReload(f_hal_nfc_timers[timer].timer, 0);
    if(LL_TIM_IsActiveFlag_UPDATE(f_hal_nfc_timers[timer].timer)) {
        LL_TIM_ClearFlag_UPDATE(f_hal_nfc_timers[timer].timer);
    }
    furi_hal_gpio_write(f_hal_nfc_timers[timer].pin, false);
}

void f_hal_nfc_timers_init() {
    for(size_t i = 0; i < FHalNfcTimerCount; i++) {
        f_hal_nfc_timer_init(i);
    }
}

void f_hal_nfc_timers_deinit() {
    for(size_t i = 0; i < FHalNfcTimerCount; i++) {
        f_hal_nfc_timer_deinit(i);
    }
}

void f_hal_nfc_timer_fwt_start(uint32_t time_fc) {
    furi_check(f_hal_nfc_timers[FHalNfcTimerFwt].is_configured);
    f_hal_nfc_timer_start(FHalNfcTimerFwt, time_fc);
}

void f_hal_nfc_timer_fwt_stop() {
    furi_check(f_hal_nfc_timers[FHalNfcTimerFwt].is_configured);
    f_hal_nfc_timer_stop(FHalNfcTimerFwt);
}

void f_hal_nfc_timer_block_tx_start(uint32_t time_fc) {
    furi_check(f_hal_nfc_timers[FHalNfcTimerBlockTx].is_configured);
    furi_check(!f_hal_nfc_timer_block_tx_is_running());

    f_hal_nfc_timer_start(FHalNfcTimerBlockTx, time_fc);
}

void f_hal_nfc_timer_block_tx_start_us(uint32_t time_us) {
    furi_check(f_hal_nfc_timers[FHalNfcTimerBlockTx].is_configured);
    furi_check(!f_hal_nfc_timer_block_tx_is_running());

    uint32_t arr_reg = f_hal_nfc_timers[FHalNfcTimerBlockTx].freq_khz / 1000 * time_us;
    furi_check(arr_reg < UINT16_MAX);

    LL_TIM_SetAutoReload(f_hal_nfc_timers[FHalNfcTimerBlockTx].timer, arr_reg);
    LL_TIM_EnableCounter(f_hal_nfc_timers[FHalNfcTimerBlockTx].timer);
    furi_hal_gpio_write(f_hal_nfc_timers[FHalNfcTimerBlockTx].pin, true);
}

void f_hal_nfc_timer_block_tx_stop() {
    furi_check(f_hal_nfc_timers[FHalNfcTimerBlockTx].is_configured);
    f_hal_nfc_timer_stop(FHalNfcTimerBlockTx);
}

bool f_hal_nfc_timer_block_tx_is_running() {
    return LL_TIM_IsEnabledCounter(f_hal_nfc_timers[FHalNfcTimerBlockTx].timer) == 1;
}
