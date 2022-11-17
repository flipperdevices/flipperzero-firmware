#include <furi.h>
#include <furi_hal.h>
#include "zero_tracker.h"

/**
 * @brief Note record
 * 
 * AH       AL
 * FEDCBA98 76543210
 * nnnnnnee eedddddd
 * -------- --------
 * nnnnnn            = [0] note kill, [1..60] note number, [61..63] service values
 *       ee ee       = [0..F] effect 
 *            111222 = [0..63] or [0..7, 0..7] effect data 
 */
typedef uint16_t NoteRecord;

uint8_t record_get_note(NoteRecord note) {
    return note & 0x3F;
}

uint8_t record_get_effect(NoteRecord note) {
    return (note >> 6) & 0xF;
}

uint8_t record_get_effect_data(NoteRecord note) {
    return (note >> 10) & 0x3F;
}

#define PATTERN_SIZE 64

typedef struct {
    NoteRecord notes[PATTERN_SIZE];
} NoteRow;

typedef struct {
    uint8_t row_count;
    NoteRow* rows;
} NotePattern;

#define FURI_HAL_SPEAKER_TIMER TIM16
#define FURI_HAL_SPEAKER_CHANNEL LL_TIM_CHANNEL_CH1
#define FURI_HAL_SPEAKER_PRESCALER 500

void tracker_speaker_play(float frequency, float pwm) {
    uint32_t autoreload = (SystemCoreClock / FURI_HAL_SPEAKER_PRESCALER / frequency) - 1;
    if(autoreload < 2) {
        autoreload = 2;
    } else if(autoreload > UINT16_MAX) {
        autoreload = UINT16_MAX;
    }

    if(pwm < 0) pwm = 0;
    if(pwm > 1) pwm = 1;

    uint32_t compare_value = pwm * autoreload;

    if(compare_value == 0) {
        compare_value = 1;
    }

    LL_TIM_SetAutoReload(FURI_HAL_SPEAKER_TIMER, autoreload);
    LL_TIM_OC_SetCompareCH1(FURI_HAL_SPEAKER_TIMER, compare_value);
    LL_TIM_EnableAllOutputs(FURI_HAL_SPEAKER_TIMER);
}

void tracker_speaker_stop() {
    LL_TIM_DisableAllOutputs(FURI_HAL_SPEAKER_TIMER);
}

void tracker_speaker_init() {
    furi_hal_speaker_start(200.0f, 0.01f);
    tracker_speaker_stop();
}

void tracker_speaker_deinit() {
    furi_hal_speaker_stop();
}

void tracker_interrupt_init(FuriHalInterruptISR isr, void* context) {
    furi_hal_interrupt_set_isr(FuriHalInterruptIdTIM2, isr, context);

    // setup TIM2 to genereate update event approximately 120 times per second
    // Timer: base
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    // Prescaler to get 1kHz clock
    TIM_InitStruct.Prescaler = SystemCoreClock / 1000000 - 1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    // Auto reload to get 120Hz interrupt
    TIM_InitStruct.Autoreload = 8333 - 1;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM2, &TIM_InitStruct);
    LL_TIM_EnableIT_UPDATE(TIM2);
    LL_TIM_EnableAllOutputs(TIM2);
    LL_TIM_EnableCounter(TIM2);
}

void tracker_interrupt_deinit() {
    FURI_CRITICAL_ENTER();
    LL_TIM_DeInit(TIM2);
    FURI_CRITICAL_EXIT();
}

void tracker_interrupt_cb(void* context) {
    UNUSED(context);

    if(LL_TIM_IsActiveFlag_UPDATE(TIM2)) {
        LL_TIM_ClearFlag_UPDATE(TIM2);
    }
}

int32_t zero_tracker_app(void* p) {
    UNUSED(p);

    tracker_speaker_init();
    tracker_interrupt_init(tracker_interrupt_cb, NULL);

    while(1) {
        furi_delay_ms(1000);
    }

    return 0;
}