#include "freqs.h"

const uint32_t frequency_table[FREQ_TAB_SIZE] = {
    (uint32_t)(2093.00 * 1024), // 7th octave, the highest in this tracker
    (uint32_t)(2217.46 * 1024), // frequency precision is 1 / 1024th of Hz
    (uint32_t)(2349.32 * 1024),
    (uint32_t)(2489.02 * 1024),
    (uint32_t)(2637.02 * 1024),
    (uint32_t)(2793.83 * 1024),
    (uint32_t)(2959.96 * 1024),
    (uint32_t)(3135.96 * 1024),
    (uint32_t)(3322.44 * 1024),
    (uint32_t)(3520.00 * 1024),
    (uint32_t)(3729.31 * 1024),
    (uint32_t)(3951.07 * 1024),
};

uint32_t get_freq(uint16_t note) {
    if(note >= ((FREQ_TAB_SIZE * 8) << 8)) {
        return frequency_table[FREQ_TAB_SIZE - 1];
    }

    if((note & 0xff) == 0) {
        return frequency_table[((note >> 8) % 12)] /
               (2 << (((NUM_OCTAVES) - ((note >> 8) / 12)) - 2)); // wrap to one octave
    }

    else {
        uint64_t f1 = frequency_table[((note >> 8) % 12)] /
                      (uint64_t)(2 << (((NUM_OCTAVES) - ((note >> 8) / 12)) - 2));
        uint64_t f2 = frequency_table[(((note >> 8) + 1) % 12)] /
                      (uint64_t)(2 << (((NUM_OCTAVES) - (((note >> 8) + 1) / 12)) - 2));

        return f1 + (uint64_t)((f2 - f1) * (uint64_t)(note & 0xff)) / (uint64_t)256;
    }
}