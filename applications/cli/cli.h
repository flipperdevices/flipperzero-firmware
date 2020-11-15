#pragma once

typedef struct Cli Cli;

typedef void (*CliCallback)(char* args, void* context);

void cli_add_command(Cli* cli, const char* name, CliCallback command, void* context);

void cli_print(const char* data);
