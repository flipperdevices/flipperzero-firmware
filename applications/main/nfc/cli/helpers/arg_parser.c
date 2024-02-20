#include "arg_parser.h"

#include <furi/furi.h>
#include <toolbox/args.h>
#include <m-array.h>

ARRAY_DEF(ArgParserArgumentArray, ArgParserArgument, M_POD_OPLIST);

struct ArgParser {
    FuriString* arg_str;
    ArgParserArgumentArray_t arg_arr;
    ArgParserArgumentType mandatory_arg_type;
};

ArgParser* arg_parser_alloc() {
    ArgParser* instance = malloc(sizeof(ArgParser));
    instance->arg_str = furi_string_alloc();
    ArgParserArgumentArray_init(instance->arg_arr);

    return instance;
}

void arg_parser_free(ArgParser* instance) {
    furi_assert(instance);

    furi_string_free(instance->arg_str);
    ArgParserArgumentArray_clear(instance->arg_arr);
}

void arg_parser_add_argument(
    ArgParser* instance,
    char arg_symbol,
    bool is_optional,
    ArgParserArgumentType type) {
    furi_assert(instance);
    UNUSED(arg_symbol);
    UNUSED(is_optional);
    UNUSED(type);
}

void arg_parser_set_mandatory_argument_type(ArgParser* instance, ArgParserArgumentType type) {
    furi_assert(instance);
    UNUSED(type);
}

bool arg_parser_parse(ArgParser* instance, FuriString* str) {
    furi_assert(instance);
    UNUSED(str);
    bool success = false;

    return success;
}

const ArgParserArgument* arg_parser_get_next_argument(ArgParser* instance) {
    furi_assert(instance);

    return NULL;
}
