#pragma once

#include <cli/cli.h>

#define F0FORTH_CMD "forth"

void f0forth_cmd(Cli* cli, FuriString* args, void* context);

typedef struct {
    Cli* cli;
} F0ForthCliState;

extern F0ForthCliState* f0f_cli_state;
