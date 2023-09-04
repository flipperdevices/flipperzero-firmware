#include "version.h"
#include "../../cli_helpers.h"
#include "../../../version.h"

#ifdef TOTP_CLI_RICH_HELP_ENABLED
void totp_cli_command_version_docopt_commands() {
    TOTP_CLI_PRINTF("  " TOTP_CLI_COMMAND_VERSION "          Get application version\r\n");
}
void totp_cli_command_version_docopt_usage() {
    TOTP_CLI_PRINTF("  " TOTP_CLI_COMMAND_NAME " " TOTP_CLI_COMMAND_VERSION "\r\n");
}
#endif

void totp_cli_command_version_handle() {
    TOTP_CLI_PRINTF(
        "%" PRIu8 ".%" PRIu8 ".%" PRIu8 "\r\n",
        TOTP_APP_VERSION_MAJOR,
        TOTP_APP_VERSION_MINOR,
        TOTP_APP_VERSION_PATCH);
}