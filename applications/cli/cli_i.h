#pragma once

#include "cli.h"

#include <m-array.h>
#include <m-string.h>

#define CLI_LINE_SIZE_MAX 

ARRAY_DEF(CliCommandArray, CliCommand*, M_PTR_OPLIST);

typedef enum {
    CliSymbolAsciiBell = 0x07,
    CliSymbolAsciiBackspace = 0x08,
    CliSymbolAsciiTab = 0x09,
    CliSymbolAsciiCR = 0x0D,
    CliSymbolAsciiEsc = 0x1B,
    CliSymbolAsciiUS = 0x1F,
    CliSymbolAsciiSpace = 0x20,
    CliSymbolAsciiDel = 0x7F,
} CliSymbols;

typedef enum {
    CliStateEmpty,
    CliStateSome,
    CliStateExecuting
} CliState;

struct Cli {
    CliCommandArray_t commands;
    string_t line;
    size_t caret;
    CliState state;
};

Cli* cli_alloc();
void cli_free(Cli* cli);
void cli_reset_state(Cli* cli);
void cli_putc(char c);
