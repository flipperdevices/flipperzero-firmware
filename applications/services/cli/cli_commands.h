#pragma once

#include "cli.h"
#include <flipper_application/flipper_application.h>

void cli_commands_init(Cli* cli);

#define PLUGIN_APP_ID      "cli"
#define PLUGIN_API_VERSION 1

typedef struct {
    char* name;
    CliExecuteCallback execute_callback;
    CliCommandFlag flags;
    size_t stack_depth;
} CliCommandDescriptor;

#define CLI_COMMAND_INTERFACE(name, execute_callback, flags, stack_depth) \
    static const CliCommandDescriptor cli_##name##_desc = {               \
        #name,                                                            \
        &execute_callback,                                                \
        flags,                                                            \
        stack_depth,                                                      \
    };                                                                    \
                                                                          \
    static const FlipperAppPluginDescriptor plugin_descriptor = {         \
        .appid = PLUGIN_APP_ID,                                           \
        .ep_api_version = PLUGIN_API_VERSION,                             \
        .entry_point = &cli_##name##_desc,                                \
    };                                                                    \
                                                                          \
    const FlipperAppPluginDescriptor* cli_##name##_ep(void) {             \
        return &plugin_descriptor;                                        \
    }
