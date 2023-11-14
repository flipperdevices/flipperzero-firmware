#pragma once

#include "../tracker_engine/tracker_engine_defs.h"
#include <stdio.h>

typedef struct {
    uint16_t opcode;
    uint16_t mask;
    char *name, *shortname;
} OpcodeDescription;

char* get_opcode_description(uint16_t opcode, bool short_description);