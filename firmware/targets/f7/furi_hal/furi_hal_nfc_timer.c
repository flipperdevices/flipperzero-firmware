#include "furi_hal_nfc_i.h"
#include "furi_hal_nfc_tech_i.h"

#include <stm32wbxx_ll_tim.h>

#include <furi_hal_interrupt.h>
#include <furi_hal_resources.h>
#include <furi_hal_bus.h>

#define TAG "FuriHalNfcTimer"

/**
 * To enable timer debug output on GPIO, define the FURI_HAL_NFC_TIMER_DEBUG macro
 * Example: ./fbt --extra-define=FURI_HAL_NFC_TIMER_DEBUG
 */

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
#ifdef FURI_HAL_NFC_TIMER_DEBUG
    const GpioPin* pin;
#endif
} FuriHalNfcTimerConfig;

static FuriHalNfcTimerConfig furi_hal_nfc_timers[FuriHalNfcTimerCount] = {
    [FuriHalNfcTimerFwt] =
        {
#ifdef FURI_HAL_NFC_TIMER_DEBUG
            .pin = &gpio_ext_pa7,
#endif
            .timer = TIM1,
            .bus = FuriHalBusTIM1,
            .event = FuriHalNfcEventInternalTypeTimerFwtExpired,
            .irq_id = FuriHalInterruptIdTim1UpTim16,
            .irq_type = TIM1_UP_TIM16_IRQn,
            .is_configured = false,
        },
    [FuriHalNfcTimerBlockTx] =
        {
#ifdef FURI_HAL_NFC_TIMER_DEBUG
            .pin = &gpio_ext_pa6,
#endif
            .timer = TIM17,
            .bus = FuriHalBusTIM17,
            .event = FuriHalNfcEventInternalTypeTimerBlockTxExpired,
            .irq_id = FuriHalInterruptIdTim1TrgComTim17,
            .irq_type = TIM1_TRG_COM_TIM17_IRQn,
            .is_configured = false,
        },
};

static void furi_hal_nfc_timer_irq_callback(void* context) {
    FuriHalNfcTimerConfig* timer_config = context;
    if(LL_TIM_IsActiveFlag_UPDATE(timer_config->timer)) {
        LL_TIM_ClearFlag_UPDATE(timer_config->timer);
        furi_hal_nfc_event_set(timer_config->event);
#ifdef FURI_HAL_NFC_TIMER_DEBUG
        furi_hal_gpio_write(timer_config->pin, false);
#endif
    }
}

static void furi_hal_nfc_timer_init(FuriHalNfcTimer timer) {
    furi_hal_bus_enable(furi_hal_nfc_timers[timer].bus);

    LL_TIM_SetOnePulseMode(furi_hal_nfc_timers[timer].timer, LL_TIM_ONEPULSEMODE_SINGLE);
    LL_TIM_EnableUpdateEvent(furi_hal_nfc_timers[timer].timer);
    LL_TIM_SetCounterMode(furi_hal_nfc_timers[timer].timer, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetClockSource(furi_hal_nfc_timers[timer].timer, LL_TIM_CLOCKSOURCE_INTERNAL);

    furi_hal_interrupt_set_isr(
        furi_hal_nfc_timers[timer].irq_id,
        furi_hal_nfc_timer_irq_callback,
        &furi_hal_nfc_timers[timer]);
    NVIC_SetPriority(
        furi_hal_nfc_timers[timer].irq_type,
        NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(furi_hal_nfc_timers[timer].irq_type);
    furi_hal_nfc_timers[timer].is_configured = true;
#ifdef FURI_HAL_NFC_TIMER_DEBUG
    furi_hal_gpio_init(
        furi_hal_nfc_timers[timer].pin, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_write(furi_hal_nfc_timers[timer].pin, false);
#endif
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

static int32_t furi_hal_nfc_timer_get_compensation(FuriHalNfcTimer timer) {
    const FuriHalNfcTechBase* current_tech = furi_hal_nfc_tech[furi_hal_nfc.tech];

    if(furi_hal_nfc.mode == FuriHalNfcModePoller) {
        const FuriHalNfcPollerCompensation* comp = &current_tech->poller.compensation;
        if(timer == FuriHalNfcTimerFwt)
            return comp->fwt;
        else if(timer == FuriHalNfcTimerBlockTx)
            return comp->fdt;

    } else if(furi_hal_nfc.mode == FuriHalNfcModeListener) {
        const FuriHalNfcListenerCompensation* comp = &current_tech->listener.compensation;
        if(timer == FuriHalNfcTimerBlockTx) return comp->fdt;
    }

    return 0;
}

static inline bool furi_hal_nfc_timer_is_running(FuriHalNfcTimer timer) {
    return LL_TIM_IsEnabledCounter(furi_hal_nfc_timers[timer].timer) != 0;
}

static void furi_hal_nfc_timer_start_core_ticks(FuriHalNfcTimer timer, uint64_t core_ticks) {
    furi_check(!furi_hal_nfc_timer_is_running(timer));

    const uint32_t prescaler = (core_ticks - 1) / UINT16_MAX;
    furi_check(prescaler <= UINT16_MAX);

    const uint32_t arr_reg = core_ticks / (prescaler + 1);
    furi_check(arr_reg <= UINT16_MAX);

    LL_TIM_DisableIT_UPDATE(furi_hal_nfc_timers[timer].timer);
    LL_TIM_SetPrescaler(furi_hal_nfc_timers[timer].timer, prescaler);
    LL_TIM_GenerateEvent_UPDATE(furi_hal_nfc_timers[timer].timer);
    LL_TIM_ClearFlag_UPDATE(furi_hal_nfc_timers[timer].timer);

    LL_TIM_SetAutoReload(furi_hal_nfc_timers[timer].timer, arr_reg);

    LL_TIM_EnableIT_UPDATE(furi_hal_nfc_timers[timer].timer);
    LL_TIM_EnableCounter(furi_hal_nfc_timers[timer].timer);
#ifdef FURI_HAL_NFC_TIMER_DEBUG
    furi_hal_gpio_write(furi_hal_nfc_timers[timer].pin, true);
#endif
}

static void furi_hal_nfc_timer_start_us(FuriHalNfcTimer timer, uint32_t time_us) {
    furi_hal_nfc_timer_start_core_ticks(timer, SystemCoreClock / 1000000UL * time_us);
}

static void furi_hal_nfc_timer_start_fc(FuriHalNfcTimer timer, uint32_t time_fc) {
    const int32_t comp_fc = furi_hal_nfc_timer_get_compensation(timer);
    // Not starting the timer if the compensation value is greater than the requested delay
    if(comp_fc >= (int32_t)time_fc) return;

    furi_hal_nfc_timer_start_core_ticks(
        timer, ((uint64_t)SystemCoreClock * (time_fc - comp_fc)) / 13560000UL);
}

static void furi_hal_nfc_timer_stop(FuriHalNfcTimer timer) {
    LL_TIM_DisableIT_UPDATE(furi_hal_nfc_timers[timer].timer);
    LL_TIM_DisableCounter(furi_hal_nfc_timers[timer].timer);
    LL_TIM_SetCounter(furi_hal_nfc_timers[timer].timer, 0);
    LL_TIM_SetAutoReload(furi_hal_nfc_timers[timer].timer, 0);
    if(LL_TIM_IsActiveFlag_UPDATE(furi_hal_nfc_timers[timer].timer)) {
        LL_TIM_ClearFlag_UPDATE(furi_hal_nfc_timers[timer].timer);
    }
#ifdef FURI_HAL_NFC_TIMER_DEBUG
    furi_hal_gpio_write(furi_hal_nfc_timers[timer].pin, false);
#endif
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
    furi_hal_nfc_timer_start_fc(FuriHalNfcTimerFwt, time_fc);
}

void furi_hal_nfc_timer_fwt_stop() {
    furi_check(furi_hal_nfc_timers[FuriHalNfcTimerFwt].is_configured);
    furi_hal_nfc_timer_stop(FuriHalNfcTimerFwt);
}

void furi_hal_nfc_timer_block_tx_start(uint32_t time_fc) {
    furi_check(furi_hal_nfc_timers[FuriHalNfcTimerBlockTx].is_configured);
    furi_hal_nfc_timer_start_fc(FuriHalNfcTimerBlockTx, time_fc);
}

void furi_hal_nfc_timer_block_tx_start_us(uint32_t time_us) {
    furi_check(furi_hal_nfc_timers[FuriHalNfcTimerBlockTx].is_configured);
    furi_hal_nfc_timer_start_us(FuriHalNfcTimerBlockTx, time_us);
}

void furi_hal_nfc_timer_block_tx_stop() {
    furi_check(furi_hal_nfc_timers[FuriHalNfcTimerBlockTx].is_configured);
    furi_hal_nfc_timer_stop(FuriHalNfcTimerBlockTx);
}

bool furi_hal_nfc_timer_block_tx_is_running() {
    return furi_hal_nfc_timer_is_running(FuriHalNfcTimerBlockTx);
}
