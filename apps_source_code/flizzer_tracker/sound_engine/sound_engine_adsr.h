#pragma once

#include "sound_engine_defs.h"

int32_t sound_engine_cycle_and_output_adsr(
    int32_t input,
    SoundEngine* eng,
    SoundEngineADSR* adsr,
    uint16_t* flags);