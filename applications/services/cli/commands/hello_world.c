#include "../cli_commands.h"

static void execute(PipeSide* pipe, FuriString* args, void* context) {
    UNUSED(pipe);
    UNUSED(args);
    UNUSED(context);
    puts("Hello, World!");
}

CLI_COMMAND_INTERFACE(hello_world, execute, CliCommandFlagDefault, 768);
