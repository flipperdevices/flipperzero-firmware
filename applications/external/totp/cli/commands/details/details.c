#include "details.h"
#include <stdlib.h>
#include <lib/toolbox/args.h>
#include "../../../types/token_info.h"
#include "../../../services/config/constants.h"
#include "../../../services/config/config.h"
#include "../../../ui/scene_director.h"
#include "../../cli_helpers.h"
#include "../../common_command_arguments.h"
#include "formatters/table/details_output_formatter_table.h"
#include "formatters/tsv/details_output_formatter_tsv.h"

typedef void (*TOTP_CLI_DETAILS_HEADER_FORMATTER)();
typedef void (*TOTP_CLI_DETAILS_FOOTER_FORMATTER)();
typedef void (*TOTP_CLI_DETAILS_AUTOMATION_FEATURE_ITEM_FORMATTER)(
    const char* key,
    const char* feature,
    bool* header_printed);
typedef void (*TOTP_CLI_DETAILS_CSTR_FORMATTER)(const char* key, const char* value);
typedef void (*TOTP_CLI_DETAILS_UINT8T_FORMATTER)(const char* key, uint8_t value);
typedef void (*TOTP_CLI_DETAILS_SIZET_FORMATTER)(const char* key, size_t value);

typedef struct {
    const TOTP_CLI_DETAILS_HEADER_FORMATTER header_formatter;
    const TOTP_CLI_DETAILS_FOOTER_FORMATTER footer_formatter;
    const TOTP_CLI_DETAILS_AUTOMATION_FEATURE_ITEM_FORMATTER automation_feature_item_formatter;
    const TOTP_CLI_DETAILS_CSTR_FORMATTER cstr_formatter;
    const TOTP_CLI_DETAILS_UINT8T_FORMATTER uint8t_formatter;
    const TOTP_CLI_DETAILS_SIZET_FORMATTER sizet_formatter;
} TotpCliDetailsFormatter;

static const TotpCliDetailsFormatter available_formatters[] = {
    {.header_formatter = &details_output_formatter_print_header_table,
     .footer_formatter = &details_output_formatter_print_footer_table,
     .automation_feature_item_formatter = &details_output_formatter_print_automation_feature_table,
     .cstr_formatter = &details_output_formatter_print_cstr_table,
     .uint8t_formatter = &details_output_formatter_print_uint8t_table,
     .sizet_formatter = &details_output_formatter_print_sizet_table},

    {.header_formatter = &details_output_formatter_print_header_tsv,
     .footer_formatter = &details_output_formatter_print_footer_tsv,
     .automation_feature_item_formatter = &details_output_formatter_print_automation_feature_tsv,
     .cstr_formatter = &details_output_formatter_print_cstr_tsv,
     .uint8t_formatter = &details_output_formatter_print_uint8t_tsv,
     .sizet_formatter = &details_output_formatter_print_sizet_tsv},
};

static void print_automation_features(
    const TokenInfo* token_info,
    const TotpCliDetailsFormatter* formatter) {
    bool header_printed = false;
    const char* AUTOMATION_FEATURES_PRINT_KEY = "Automation features";
    if(token_info->automation_features == TokenAutomationFeatureNone) {
        (*formatter->automation_feature_item_formatter)(
            AUTOMATION_FEATURES_PRINT_KEY, "None", &header_printed);
        return;
    }

    if(token_info->automation_features & TokenAutomationFeatureEnterAtTheEnd) {
        (*formatter->automation_feature_item_formatter)(
            AUTOMATION_FEATURES_PRINT_KEY, "Type <Enter> key at the end", &header_printed);
    }

    if(token_info->automation_features & TokenAutomationFeatureTabAtTheEnd) {
        (*formatter->automation_feature_item_formatter)(
            AUTOMATION_FEATURES_PRINT_KEY, "Type <Tab> key at the end", &header_printed);
    }

    if(token_info->automation_features & TokenAutomationFeatureTypeSlower) {
        (*formatter->automation_feature_item_formatter)(
            AUTOMATION_FEATURES_PRINT_KEY, "Type slower", &header_printed);
    }
}

#ifdef TOTP_CLI_RICH_HELP_ENABLED
void totp_cli_command_details_docopt_commands() {
    TOTP_CLI_PRINTF("  " TOTP_CLI_COMMAND_DETAILS ", " TOTP_CLI_COMMAND_DETAILS_ALT
                    "      Displays token details\r\n");
}

void totp_cli_command_details_docopt_usage() {
    TOTP_CLI_PRINTF("  " TOTP_CLI_COMMAND_NAME " " DOCOPT_REQUIRED(
        TOTP_CLI_COMMAND_DETAILS
        " | " TOTP_CLI_COMMAND_DETAILS_ALT) " " DOCOPT_ARGUMENT(TOTP_CLI_COMMAND_ARG_INDEX) "\r\n");
}
#endif

void totp_cli_command_details_handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    if(!totp_cli_ensure_authenticated(plugin_state, cli)) {
        return;
    }

    int token_number;
    TokenInfoIteratorContext* iterator_context =
        totp_config_get_token_iterator_context(plugin_state);
    if(!args_read_int_and_trim(args, &token_number) || token_number <= 0 ||
       (size_t)token_number > totp_token_info_iterator_get_total_count(iterator_context)) {
        totp_cli_print_invalid_arguments();
        return;
    }

    const TotpCliDetailsFormatter* formatter = &available_formatters[0];
    FuriString* arg = furi_string_alloc();
    if(args_read_string_and_trim(args, arg) && furi_string_cmpi_str(arg, "--tsv") == 0) {
        formatter = &available_formatters[1];
    }

    furi_string_free(arg);

    TOTP_CLI_LOCK_UI(plugin_state);

    size_t original_token_index =
        totp_token_info_iterator_get_current_token_index(iterator_context);
    if(totp_token_info_iterator_go_to(iterator_context, token_number - 1)) {
        const TokenInfo* token_info = totp_token_info_iterator_get_current_token(iterator_context);

        (*formatter->header_formatter)();
        (*formatter->sizet_formatter)("Index", token_number);
        (*formatter->cstr_formatter)("Name", furi_string_get_cstr(token_info->name));
        (*formatter->cstr_formatter)("Hashing algorithm", token_info_get_algo_as_cstr(token_info));
        (*formatter->uint8t_formatter)("Number of digits", token_info->digits);
        (*formatter->uint8t_formatter)("Token lifetime", token_info->duration);
        print_automation_features(token_info, formatter);
        (*formatter->footer_formatter)();
    } else {
        totp_cli_print_error_loading_token_info();
    }

    totp_token_info_iterator_go_to(iterator_context, original_token_index);

    TOTP_CLI_UNLOCK_UI(plugin_state);
}