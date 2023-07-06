#pragma once

#include "sound_engine/sound_engine.h"
#include "tracker_engine/tracker_engine.h"

#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_gpio.h>
#include <stm32wbxx_ll_tim.h>

#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#define SPEAKER_PWM_TIMER TIM16
#define SAMPLE_RATE_TIMER TIM1
#define TRACKER_ENGINE_TIMER TIM2

#define SPEAKER_PWM_TIMER_CHANNEL LL_TIM_CHANNEL_CH1

#define TIMER_BASE_CLOCK 64000000 /* CPU frequency, 64 MHz */

#define DMA_INSTANCE DMA1, LL_DMA_CHANNEL_1

void sound_engine_dma_isr(void* ctx);
void tracker_engine_timer_isr(void* ctx);
void sound_engine_init_hardware(
    uint32_t sample_rate,
    bool external_audio_output,
    uint16_t* audio_buffer,
    uint32_t audio_buffer_size);
void sound_engine_dma_init(uint32_t address, uint32_t size);
void sound_engine_PWM_timer_init(bool external_audio_output);
void sound_engine_set_audio_output(bool external_audio_output);
void tracker_engine_init_hardware(uint8_t rate);
void tracker_engine_timer_init(uint8_t rate);
void tracker_engine_set_rate(uint8_t rate);
void sound_engine_start();
void sound_engine_stop();
void stop();
void play();
void tracker_engine_stop();
void sound_engine_deinit_timer();
void tracker_engine_start();