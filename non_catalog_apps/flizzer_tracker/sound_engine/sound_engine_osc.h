#pragma once

#include "sound_engine_defs.h"

uint16_t sound_engine_triangle(uint32_t acc);

uint16_t
    sound_engine_osc(SoundEngine* sound_engine, SoundEngineChannel* channel, uint32_t prev_acc);