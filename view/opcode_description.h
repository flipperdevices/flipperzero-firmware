#pragma once

#include <stdio.h>
#include "../tracker_engine/tracker_engine_defs.h"

typedef struct
{
    uint16_t opcode;
    uint16_t mask;
    char *name, *shortname;
} OpcodeDescription;

char *get_opcode_description(uint16_t opcode, bool short_description);