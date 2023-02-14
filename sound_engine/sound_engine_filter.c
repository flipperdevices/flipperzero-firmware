#include "sound_engine_filter.h"

void sound_engine_filter_set_coeff(SoundEngineFilter* flt, uint32_t frequency, uint16_t resonance) {
    flt->cutoff = (frequency << 5);
    flt->resonance = ((int32_t)resonance * 11 / 6) - 200;
}

void sound_engine_filter_cycle(
    SoundEngineFilter* flt,
    int32_t input) // don't ask me how it works, stolen from Furnace tracker TSU synth
{
    input /= 8;
    flt->low = flt->low + ((flt->cutoff * flt->band) >> 16);
    flt->high = input - flt->low - (((256 - flt->resonance) * flt->band) >> 8);
    flt->band = ((flt->cutoff * flt->high) >> 16) + flt->band;
}

int32_t sound_engine_output_lowpass(SoundEngineFilter* flt) {
    return flt->low * 8;
}

int32_t sound_engine_output_highpass(SoundEngineFilter* flt) {
    return flt->high * 8;
}

int32_t sound_engine_output_bandpass(SoundEngineFilter* flt) {
    return flt->band * 8;
}