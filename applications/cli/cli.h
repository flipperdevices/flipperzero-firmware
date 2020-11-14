#pragma once

typedef struct Cli Cli;

typedef void (*CliCallback)(char *args);

typedef struct {
    const char* name;
    const char* help;
    CliCallback callback;
} CliCommand;

void cli_add_command(CliCommand command);

void cli_printf();
