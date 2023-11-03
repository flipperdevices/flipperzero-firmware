#pragma once
#include <stdint.h>
#include "types.h"

#define HOUSE_LINE_CO 11
#define CRANE_LINE_CO 1
#define GROUND_LINE_CO 2

typedef struct {
    uint8_t count;
    // We have to free this later!
    Line* lines;
} Block;

Block get_house();
Block get_crane();
Block get_ground();
