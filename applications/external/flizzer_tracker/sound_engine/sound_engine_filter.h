#pragma once

#include "sound_engine_defs.h"

void sound_engine_filter_set_coeff(SoundEngineFilter* flt, uint32_t frequency, uint16_t resonance);
void sound_engine_filter_cycle(SoundEngineFilter* flt, int32_t input);
int32_t sound_engine_output_lowpass(SoundEngineFilter* flt);
int32_t sound_engine_output_highpass(SoundEngineFilter* flt);
int32_t sound_engine_output_bandpass(SoundEngineFilter* flt);