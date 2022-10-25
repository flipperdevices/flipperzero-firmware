#pragma once

#define TOTP_CLI_ARG(arg) "<" arg ">"
#define TOTP_CLI_OPTIONAL_PARAM(param) "[" param "]"
#define TOTP_CLI_OPTIONAL_PARAM_MARK "[OPTIONAL]"

#define TOTP_CLI_PRINTF(format, ...) \
    _Pragma(STRINGIFY(GCC diagnostic push)); \
    _Pragma(STRINGIFY(GCC diagnostic ignored "-Wdouble-promotion")); \
    printf(format, ##__VA_ARGS__); \
    _Pragma(STRINGIFY(GCC diagnostic pop));

void totp_cli_print_invalid_arguments();