#include "dolphin_state.h"

const uint32_t DOLPHIN_LEVELS[DOLPHIN_LEVEL_COUNT] = {500,    1250,   2250,   3500,   5000,  6750,
                                                      8750,   11000,  13500,  16250,  19250, 22500,
                                                      26000,  29750,  33750,  38000,  42500, 47250,
                                                      52250,  58250,  65250,  73250,  82250, 92250,
                                                      103250, 115250, 128250, 142250, 157250};

uint8_t doc_dolphin_get_level(uint32_t icounter) {
    for(int i = 0; i < DOLPHIN_LEVEL_COUNT; ++i) {
        if(icounter <= DOLPHIN_LEVELS[i]) {
            return i + 1;
        }
    }
    return DOLPHIN_LEVEL_COUNT + 1;
}

uint32_t doc_dolphin_state_xp_to_levelup(uint32_t icounter) {
    for(int i = 0; i < DOLPHIN_LEVEL_COUNT; ++i) {
        if(icounter <= DOLPHIN_LEVELS[i]) {
            return DOLPHIN_LEVELS[i] - icounter;
        }
    }
    return (uint32_t)-1;
}