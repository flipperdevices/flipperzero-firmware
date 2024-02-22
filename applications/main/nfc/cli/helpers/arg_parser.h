#pragma once

#include <stdint.h>
#include <stddef.h>
#include <furi/core/string.h>

typedef struct {
    const char identifier;
    const char* access_letters;
    const char* access_name;
    const char* value_name;
    const char* description;
} ArgParserOptions;

typedef struct ArgParser ArgParser;

ArgParser* arg_parser_alloc(
    const ArgParserOptions* options,
    size_t option_count,
    const FuriString* command);

void arg_parser_free(ArgParser* instance);

bool arg_parser_fetch(ArgParser* instance);

char arg_parser_get_identifier(ArgParser* instance);

const char* arg_parser_get_value(ArgParser* instance);

void arg_parser_get_error_message(ArgParser* instance, FuriString* error_str);

void arg_parser_get_help_message(ArgParser* instance, FuriString* help_str);

const char* arg_parser_get_next_argument(ArgParser* instance);
