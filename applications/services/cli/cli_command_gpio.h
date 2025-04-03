#pragma once

#include "cli_i.h"
#include <toolbox/pipe.h>

void cli_command_gpio(PipeSide* pipe, FuriString* args, void* context);
