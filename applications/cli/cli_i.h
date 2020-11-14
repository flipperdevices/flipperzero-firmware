#pragma once

#include "cli.h"

#include <m-string.h>
#include <m-dict.h>

#define CLI_LINE_SIZE_MAX

DICT_DEF2(CliCommandDict, string_t, STRING_OPLIST, CliCallback, M_PTR_OPLIST)

typedef enum {
    CliSymbolAsciiSOH = 0x01,
    CliSymbolAsciiEOT = 0x04,
    CliSymbolAsciiBell = 0x07,
    CliSymbolAsciiBackspace = 0x08,
    CliSymbolAsciiTab = 0x09,
    CliSymbolAsciiCR = 0x0D,
    CliSymbolAsciiEsc = 0x1B,
    CliSymbolAsciiUS = 0x1F,
    CliSymbolAsciiSpace = 0x20,
    CliSymbolAsciiDel = 0x7F,
} CliSymbols;

typedef enum { CliStateIdle, CliStateRunning } CliState;

struct Cli {
    CliCommandDict_t commands;
    string_t line;
    CliState state;
};

Cli* cli_alloc();
void cli_free(Cli* cli);
void cli_reset_state(Cli* cli);
void cli_putc(char c);
