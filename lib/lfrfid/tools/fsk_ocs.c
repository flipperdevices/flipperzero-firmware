#include "fsk_osc.h"
#include <stdlib.h>

struct FSKOsc {
    uint16_t freq[2];
    uint16_t osc_phase_max;
    int32_t osc_phase_current;
};

FSKOsc* fsk_osc_alloc(uint32_t freq_low, uint32_t freq_hi, uint32_t osc_phase_max) {
    FSKOsc* osc = malloc(sizeof(FSKOsc));
    osc->freq[0] = freq_low;
    osc->freq[1] = freq_hi;
    osc->osc_phase_max = osc_phase_max;
    osc->osc_phase_current = 0;
    return osc;
}

void fsk_osc_free(FSKOsc* osc) {
    free(osc);
}

bool fsk_osc_next(FSKOsc* osc, bool bit, uint32_t* period) {
    bool advance = false;
    *period = osc->freq[bit];
    osc->osc_phase_current += *period;

    if(osc->osc_phase_current > osc->osc_phase_max) {
        advance = true;
        osc->osc_phase_current -= osc->osc_phase_max;
    }

    return advance;
}
