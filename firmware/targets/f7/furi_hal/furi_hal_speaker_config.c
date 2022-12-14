//#include <furi_hal_speaker.h>
#include <furi_hal_speaker_config.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include <stm32wbxx_ll_tim.h>

FuriMutex* furi_hal_speaker_bus_mutex = NULL;

static void furi_hal_speaker_busr_event(FuriHalSpeakerBus* bus, FuriHalSpeakerBusEvent event) {
    if(event == FuriHalSpeakerBusEventInit) {
        furi_hal_speaker_bus_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
        FURI_CRITICAL_ENTER();
        LL_TIM_DeInit(FURI_HAL_SPEAKER_TIMER);
        FURI_CRITICAL_EXIT();
        furi_hal_gpio_init_ex(
            &gpio_speaker, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn14TIM16);
        bus->current_handle = NULL;
    } else if(event == FuriHalSpeakerBusEventDeinit) {
        furi_mutex_free(furi_hal_speaker_bus_mutex);
        FURI_CRITICAL_ENTER();
        LL_TIM_DisableAllOutputs(FURI_HAL_SPEAKER_TIMER);
        LL_TIM_DisableCounter(FURI_HAL_SPEAKER_TIMER);
        FURI_CRITICAL_EXIT();
        furi_hal_gpio_init(&gpio_speaker, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    } else if(event == FuriHalSpeakerBusEventLock) {
        furi_check(
            furi_mutex_acquire(furi_hal_speaker_bus_mutex, FuriWaitForever) == FuriStatusOk);
    } else if(event == FuriHalSpeakerBusEventUnlock) {
        furi_check(furi_mutex_release(furi_hal_speaker_bus_mutex) == FuriStatusOk);
    } else if(event == FuriHalSpeakerBusEventActivate) {
        FURI_CRITICAL_ENTER();
        furi_hal_gpio_init_ex(
            &gpio_speaker, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn14TIM16);
        FURI_CRITICAL_EXIT();
    } else if(event == FuriHalSpeakerBusEventDeactivate) {
        FURI_CRITICAL_ENTER();
        furi_hal_gpio_init(&gpio_speaker, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
        FURI_CRITICAL_EXIT();
    }
}

FuriHalSpeakerBus furi_hal_speaker_bus = {
    .callback = furi_hal_speaker_busr_event,
};

void furi_hal_speaker_bus_handle_event(
    FuriHalSpeakerBusHandle* handle,
    FuriHalSpeakerBusHandleEvent event) {
    UNUSED(handle);
    if(event == FuriHalSpeakerBusHandleEventActivate) {
        furi_hal_gpio_init_ex(
            &gpio_speaker, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn14TIM16);
    } else if(event == FuriHalSpeakerBusHandleEventDeactivate) {
        LL_TIM_DisableAllOutputs(FURI_HAL_SPEAKER_TIMER);
        LL_TIM_DisableCounter(FURI_HAL_SPEAKER_TIMER);
        furi_hal_gpio_init(&gpio_speaker, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    }
}

FuriHalSpeakerBusHandle furi_hal_speaker_handle = {
    .bus = &furi_hal_speaker_bus,
    .callback = furi_hal_speaker_bus_handle_event,
};