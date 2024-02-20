#pragma once

#include <stdint.h>
#include <stddef.h>
#include <furi/core/string.h>

typedef enum {
    ArgParserArgumentTypeBool,
    ArgParserArgumentTypeUint32,
    ArgParserArgumentTypeHexArray,
} ArgParserArgumentType;

typedef struct {
    ArgParserArgumentType type;
    union {
        bool* data_bool;
        uint32_t* data_uint32;
        uint8_t* data_array;
    };
} ArgParserArgument;

typedef struct ArgParser ArgParser;

ArgParser* arg_parser_alloc();

void arg_parser_free(ArgParser* instance);

void arg_parser_add_argument(
    ArgParser* instance,
    char arg_symbol,
    bool is_optional,
    ArgParserArgumentType type);

void arg_parser_set_mandatory_argument_type(ArgParser* instance, ArgParserArgumentType type);

bool arg_parser_parse(ArgParser* instance, FuriString* str);

const ArgParserArgument* arg_parser_get_next_argument(ArgParser* instance);
