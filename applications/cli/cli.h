#pragma once

typedef struct Cli Cli;

typedef void (*CliCallback)(char* args);

void cli_add_command(Cli* cli, const char* name, CliCallback command);
