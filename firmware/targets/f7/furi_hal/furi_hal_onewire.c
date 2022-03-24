#include <furi_hal_onewire.h>
#include <furi_hal_interrupt.h>

#include <stm32wbxx_ll_tim.h>

#include <furi.h>

#define FURI_HAL_ONEWIRE_TIMER TIM1
#define FURI_HAL_ONEWIRE_TIMER_IRQ TIM1_UP_TIM16_IRQn

typedef enum {
    FuriHalOnewireStateIdle,
    FuriHalOnewireStateRunning,
} FuriHalOnewireState;

typedef struct {
    FuriHalOnewireState state;
    FuriHalOnewireEmulateCallback callback;
    void* context;
} FuriHalOnewire;

FuriHalOnewire* furi_hal_onewire = NULL;

static void furi_hal_onewire_emulate_isr() {
    if(LL_TIM_IsActiveFlag_UPDATE(FURI_HAL_ONEWIRE_TIMER)) {
        LL_TIM_ClearFlag_UPDATE(FURI_HAL_ONEWIRE_TIMER);
        furi_hal_onewire->callback(furi_hal_onewire->context);
    }
}

void furi_hal_onewire_init() {
    furi_hal_onewire = malloc(sizeof(FuriHalOnewire));
    furi_hal_onewire->state = FuriHalOnewireStateIdle;
}

void furi_hal_onewire_emulate_start(
    uint32_t period,
    FuriHalOnewireEmulateCallback callback,
    void* context) {
    furi_assert(furi_hal_onewire);
    furi_assert(furi_hal_onewire->state == FuriHalOnewireStateIdle);

    furi_hal_onewire->state = FuriHalOnewireStateRunning;
    furi_hal_onewire->callback = callback;
    furi_hal_onewire->context = context;

    FURI_CRITICAL_ENTER();
    LL_TIM_DeInit(FURI_HAL_ONEWIRE_TIMER);
    FURI_CRITICAL_EXIT();

    LL_TIM_SetPrescaler(FURI_HAL_ONEWIRE_TIMER, 0);
    LL_TIM_SetCounterMode(FURI_HAL_ONEWIRE_TIMER, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(FURI_HAL_ONEWIRE_TIMER, period);
    LL_TIM_DisableARRPreload(FURI_HAL_ONEWIRE_TIMER);
    LL_TIM_SetRepetitionCounter(FURI_HAL_ONEWIRE_TIMER, 0);

    LL_TIM_SetClockDivision(FURI_HAL_ONEWIRE_TIMER, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetClockSource(FURI_HAL_ONEWIRE_TIMER, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_GenerateEvent_UPDATE(FURI_HAL_ONEWIRE_TIMER);

    LL_TIM_EnableIT_UPDATE(FURI_HAL_ONEWIRE_TIMER);

    furi_hal_interrupt_set_timer_isr(FURI_HAL_ONEWIRE_TIMER, furi_hal_onewire_emulate_isr);

    NVIC_SetPriority(
        FURI_HAL_ONEWIRE_TIMER_IRQ, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(FURI_HAL_ONEWIRE_TIMER_IRQ);

    LL_TIM_EnableCounter(FURI_HAL_ONEWIRE_TIMER);
}

void furi_hal_onewire_emulate_set_next(uint32_t period) {
    LL_TIM_SetAutoReload(FURI_HAL_ONEWIRE_TIMER, period);
}

void furi_hal_onewire_emulate_stop() {
    furi_assert(furi_hal_onewire);

    if(furi_hal_onewire->state == FuriHalOnewireStateRunning) {
        furi_hal_onewire->state = FuriHalOnewireStateIdle;
        LL_TIM_DisableCounter(FURI_HAL_ONEWIRE_TIMER);
        furi_hal_interrupt_set_timer_isr(FURI_HAL_ONEWIRE_TIMER, NULL);

        furi_hal_onewire->callback = NULL;
        furi_hal_onewire->context = NULL;
    }
}
