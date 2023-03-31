#include "details.h"
#include <stdlib.h>
#include <lib/toolbox/args.h>
#include "../../../lib/list/list.h"
#include "../../../types/token_info.h"
#include "../../../services/config/constants.h"
#include "../../cli_helpers.h"
#include "../../common_command_arguments.h"

#define AUTOMATION_FEATURES_PROPERTY_HEADER "Automation features"

static void print_automation_features(const TokenInfo* token_info) {
    if(token_info->automation_features == TOKEN_AUTOMATION_FEATURE_NONE) {
        TOTP_CLI_PRINTF("| %-20s | %-28.28s |\r\n", "Automation features", "None");
        return;
    }

    bool header_printed = false;
    if(token_info->automation_features & TOKEN_AUTOMATION_FEATURE_ENTER_AT_THE_END) {
        TOTP_CLI_PRINTF(
            "| %-20s | %-28.28s |\r\n",
            AUTOMATION_FEATURES_PROPERTY_HEADER,
            "Type <Enter> key at the end");
        header_printed = true;
    }

    if(token_info->automation_features & TOKEN_AUTOMATION_FEATURE_TAB_AT_THE_END) {
        TOTP_CLI_PRINTF(
            "| %-20s | %-28.28s |\r\n",
            header_printed ? "" : AUTOMATION_FEATURES_PROPERTY_HEADER,
            "Type <Tab> key at the end");
        header_printed = true;
    }
}

void totp_cli_command_details_docopt_commands() {
    TOTP_CLI_PRINTF("  " TOTP_CLI_COMMAND_DETAILS ", " TOTP_CLI_COMMAND_DETAILS_ALT
                    "      Displays token details\r\n");
}

void totp_cli_command_details_docopt_usage() {
    TOTP_CLI_PRINTF("  " TOTP_CLI_COMMAND_NAME " " DOCOPT_REQUIRED(
        TOTP_CLI_COMMAND_DETAILS
        " | " TOTP_CLI_COMMAND_DETAILS_ALT) " " DOCOPT_ARGUMENT(TOTP_CLI_COMMAND_ARG_INDEX) "\r\n");
}

void totp_cli_command_details_handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    if(!totp_cli_ensure_authenticated(plugin_state, cli)) {
        return;
    }

    int token_number;
    if(!args_read_int_and_trim(args, &token_number) || token_number <= 0 ||
       token_number > plugin_state->tokens_count) {
        TOTP_CLI_PRINT_INVALID_ARGUMENTS();
        return;
    }

    ListNode* list_node = list_element_at(plugin_state->tokens_list, token_number - 1);

    TokenInfo* token_info = list_node->data;

    TOTP_CLI_PRINTF("+----------------------+------------------------------+\r\n");
    TOTP_CLI_PRINTF("| %-20s | %-28s |\r\n", "Property", "Value");
    TOTP_CLI_PRINTF("+----------------------+------------------------------+\r\n");
    TOTP_CLI_PRINTF("| %-20s | %-28d |\r\n", "Index", token_number);
    TOTP_CLI_PRINTF("| %-20s | %-28.28s |\r\n", "Name", token_info->name);
    TOTP_CLI_PRINTF(
        "| %-20s | %-28s |\r\n", "Hashing algorithm", token_info_get_algo_as_cstr(token_info));
    TOTP_CLI_PRINTF("| %-20s | %-28" PRIu8 " |\r\n", "Number of digits", token_info->digits);
    TOTP_CLI_PRINTF(
        "| %-20s | %" PRIu8 " sec.%-21s |\r\n", "Token lifetime", token_info->duration, " ");
    print_automation_features(token_info);
    TOTP_CLI_PRINTF("+----------------------+------------------------------+\r\n");
}