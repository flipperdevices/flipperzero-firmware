#include "sound_engine_filter.h"

void sound_engine_filter_set_coeff(SoundEngineFilter *flt, uint32_t frequency, uint16_t resonance)
{
    flt->q = 2048 - frequency;
    flt->p = frequency + ((int32_t)(0.8f * 2048.0f) * frequency / 2048 * flt->q) / 2048;
    flt->f = flt->p + flt->p - 2048;
    flt->q = resonance;
}

void sound_engine_filter_cycle(SoundEngineFilter *flt, int32_t input)
{
    input -= flt->q * flt->b4 / 2048; // feedback
    int32_t t1 = flt->b1;
    flt->b1 = (input + flt->b0) * flt->p / 2048 - flt->b1 * flt->f / 2048;
    int32_t t2 = flt->b2;
    flt->b2 = (flt->b1 + t1) * flt->p / 2048 - flt->b2 * flt->f / 2048;
    t1 = flt->b3;
    flt->b3 = (flt->b2 + t2) * flt->p / 2048 - flt->b3 * flt->f / 2048;
    flt->b4 = (flt->b3 + t1) * flt->p / 2048 - flt->b4 * flt->f / 2048;

    flt->b0 = input;
}

int32_t sound_engine_output_lowpass(SoundEngineFilter *flt)
{
    return flt->b4;
}

int32_t sound_engine_output_highpass(SoundEngineFilter *flt)
{
    return flt->b0 - flt->b4;
}

int32_t sound_engine_output_bandpass(SoundEngineFilter *flt)
{
    return 3 * (flt->b3 - flt->b4);
}