#include "cli_common_helpers.h"
#include <cli/cli.h>

void totp_cli_print_invalid_arguments() {
    printf("Invalid command arguments. use \"help\" command to get list of available commands");
}