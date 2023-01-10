#pragma once

#include "sound_engine/sound_engine.h"

#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_dma.h>
#include <stm32wbxx_ll_gpio.h>

#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

void sound_engine_PWM_timer_init(bool external_audio_output);
void sound_engine_timer_init(uint32_t sample_rate);
void sound_engine_dma_init(uint32_t address, uint32_t size);
void sound_engine_init_hardware(uint32_t sample_rate, bool external_audio_output, uint16_t* audio_buffer, uint32_t audio_buffer_size);
void sound_engine_dma_start();
void sound_engine_dma_stop();
void sound_engine_start();
void sound_engine_stop();