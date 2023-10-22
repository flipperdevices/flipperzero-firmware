#include "random_generator.h"

uint32_t random_uniform(uint32_t min, uint32_t max) {
    return (furi_hal_random_get() % (max - min)) + min;
}

bool toss_a_coin(uint32_t probability) {
    furi_assert(100 >= probability);
    return random_uniform(1, 101) <= probability;
}
