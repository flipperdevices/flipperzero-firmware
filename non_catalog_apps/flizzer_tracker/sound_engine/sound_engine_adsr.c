#include "sound_engine_adsr.h"

int32_t sound_engine_cycle_and_output_adsr(
    int32_t input,
    SoundEngine* eng,
    SoundEngineADSR* adsr,
    uint16_t* flags) {
    switch(adsr->envelope_state) {
    case ATTACK: {
        adsr->envelope += adsr->envelope_speed;

        if(adsr->envelope >= MAX_ADSR) {
            adsr->envelope_state = DECAY;
            adsr->envelope = MAX_ADSR;

            adsr->envelope_speed = envspd(eng, adsr->d);
        }

        break;
    }

    case DECAY: {
        if(adsr->envelope > ((uint32_t)adsr->s << 17) + adsr->envelope_speed) {
            adsr->envelope -= adsr->envelope_speed;
        }

        else {
            adsr->envelope = (uint32_t)adsr->s << 17;
            adsr->envelope_state = (adsr->s == 0) ? RELEASE : SUSTAIN;

            adsr->envelope_speed = envspd(eng, adsr->r);
        }

        break;
    }

    case SUSTAIN:
    case DONE: {
        break;
    }

    case RELEASE: {
        if(adsr->envelope > adsr->envelope_speed) {
            adsr->envelope -= adsr->envelope_speed;
        }

        else {
            adsr->envelope_state = DONE;
            *flags &= ~SE_ENABLE_GATE;
            adsr->envelope = 0;
        }

        break;
    }
    }

    return (int32_t)((int32_t)input * (int32_t)(adsr->envelope >> 10) / (int32_t)(MAX_ADSR >> 10) *
                     (int32_t)adsr->volume / (int32_t)MAX_ADSR_VOLUME);
}