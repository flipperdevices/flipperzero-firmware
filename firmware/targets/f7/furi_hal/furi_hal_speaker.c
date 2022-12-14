#include <furi_hal_speaker.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>
#include <furi_hal_power.h>

#include <stm32wbxx_ll_tim.h>
#include <furi_hal_cortex.h>

#define TAG "FuriHalSpeaker"

// #define FURI_HAL_SPEAKER_NEW_VOLUME

void furi_hal_speaker_init() {
    furi_hal_speaker_bus.callback(&furi_hal_speaker_bus, FuriHalSpeakerBusEventInit);
    FURI_LOG_I(TAG, "Init OK");
}

void furi_hal_speaker_deinit() {
    furi_hal_speaker_bus.callback(&furi_hal_speaker_bus, FuriHalSpeakerBusEventDeinit);
}

bool furi_hal_speaker_acquire(FuriHalSpeakerBusHandle* handle, uint32_t timeout) {
    furi_assert(handle);
    FuriHalCortexTimer timer = furi_hal_cortex_timer_get(timeout * 1000);
    while(handle->bus->current_handle != NULL) {
        if(furi_hal_cortex_timer_is_expired(timer)) {
            return false;
        }
    }
    furi_hal_power_insomnia_enter();
    // Lock bus access
    handle->bus->callback(handle->bus, FuriHalSpeakerBusEventLock);
    // Ensuree that no active handle set
    furi_check(handle->bus->current_handle == NULL);
    // Set current handle
    handle->bus->current_handle = handle;
    // Activate bus
    handle->bus->callback(handle->bus, FuriHalSpeakerBusEventActivate);
    // Activate handle
    handle->callback(handle, FuriHalSpeakerBusHandleEventActivate);
    return true;
}

void furi_hal_speaker_release(FuriHalSpeakerBusHandle* handle) {
    // Ensure that current handle is our handle
    furi_check(handle->bus->current_handle == handle);
    // Deactivate handle
    handle->callback(handle, FuriHalSpeakerBusHandleEventDeactivate);
    // Deactivate bus
    handle->bus->callback(handle->bus, FuriHalSpeakerBusEventDeactivate);
    // Reset current handle
    handle->bus->current_handle = NULL;
    // Unlock bus
    handle->bus->callback(handle->bus, FuriHalSpeakerBusEventUnlock);
    furi_hal_power_insomnia_exit();
}

bool furi_hal_speaker_is_mine(FuriHalSpeakerBusHandle* handle) {
    return handle->bus->current_handle == handle;
}

static inline uint32_t furi_hal_speaker_calculate_autoreload(float frequency) {
    uint32_t autoreload = (SystemCoreClock / FURI_HAL_SPEAKER_PRESCALER / frequency) - 1;
    if(autoreload < 2) {
        autoreload = 2;
    } else if(autoreload > UINT16_MAX) {
        autoreload = UINT16_MAX;
    }

    return autoreload;
}

static inline uint32_t furi_hal_speaker_calculate_compare(float volume) {
    if(volume < 0) volume = 0;
    if(volume > 1) volume = 1;
    volume = volume * volume * volume;

#ifdef FURI_HAL_SPEAKER_NEW_VOLUME
    uint32_t compare_value = volume * FURI_HAL_SPEAKER_MAX_VOLUME;
    uint32_t clip_value = volume * LL_TIM_GetAutoReload(FURI_HAL_SPEAKER_TIMER) / 2;
    if(compare_value > clip_value) {
        compare_value = clip_value;
    }
#else
    uint32_t compare_value = volume * LL_TIM_GetAutoReload(FURI_HAL_SPEAKER_TIMER) / 2;
#endif

    if(compare_value == 0) {
        compare_value = 1;
    }

    return compare_value;
}

void furi_hal_speaker_start(FuriHalSpeakerBusHandle* handle, float frequency, float volume) {
    // Ensure that current handle is our handle
    furi_check(handle->bus->current_handle == handle);

    if(volume <= 0) {
        furi_hal_speaker_stop(handle);
        return;
    }

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = FURI_HAL_SPEAKER_PRESCALER - 1;
    TIM_InitStruct.Autoreload = furi_hal_speaker_calculate_autoreload(frequency);
    LL_TIM_Init(FURI_HAL_SPEAKER_TIMER, &TIM_InitStruct);

    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_ENABLE;
    TIM_OC_InitStruct.CompareValue = furi_hal_speaker_calculate_compare(volume);
    LL_TIM_OC_Init(FURI_HAL_SPEAKER_TIMER, FURI_HAL_SPEAKER_CHANNEL, &TIM_OC_InitStruct);

    LL_TIM_EnableAllOutputs(FURI_HAL_SPEAKER_TIMER);
    LL_TIM_EnableCounter(FURI_HAL_SPEAKER_TIMER);
}

void furi_hal_speaker_set_volume(FuriHalSpeakerBusHandle* handle, float volume) {
    // Ensure that current handle is our handle
    furi_check(handle->bus->current_handle == handle);
    if(volume <= 0) {
        furi_hal_speaker_stop(handle);
        return;
    }

#if FURI_HAL_SPEAKER_CHANNEL == LL_TIM_CHANNEL_CH1
    LL_TIM_OC_SetCompareCH1(FURI_HAL_SPEAKER_TIMER, furi_hal_speaker_calculate_compare(volume));
#else
#error Invalid channel
#endif
}

void furi_hal_speaker_stop(FuriHalSpeakerBusHandle* handle) {
    // Ensure that current handle is our handle
    furi_check(handle->bus->current_handle == handle);
    LL_TIM_DisableAllOutputs(FURI_HAL_SPEAKER_TIMER);
    LL_TIM_DisableCounter(FURI_HAL_SPEAKER_TIMER);
}
