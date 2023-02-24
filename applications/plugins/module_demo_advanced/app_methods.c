#include "app_methods.h"

static uint32_t accumulator = 0;

void app_api_accumulator_set(uint32_t value) {
    accumulator = value;
}

uint32_t app_api_accumulator_get() {
    return accumulator;
}

void app_api_accumulator_add(uint32_t value) {
    accumulator += value;
}

void app_api_accumulator_sub(uint32_t value) {
    accumulator -= value;
}

void app_api_accumulator_mul(uint32_t value) {
    accumulator *= value;
}
