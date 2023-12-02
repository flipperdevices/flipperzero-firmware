#pragma once

#include "tracker_engine_defs.h"
#include <stdbool.h>
#include <stdio.h>

void do_command(
    uint16_t opcode,
    TrackerEngine* tracker_engine,
    uint8_t channel,
    uint8_t tick,
    bool from_program);