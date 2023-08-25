#include "list.h"
#include <stdlib.h>
#include <lib/toolbox/args.h>
#include "../../../types/token_info.h"
#include "../../../services/config/constants.h"
#include "../../../services/config/config.h"
#include "../../../ui/scene_director.h"
#include "../../cli_helpers.h"
#include "formatters/table/list_output_formatter_table.h"
#include "formatters/tsv/list_output_formatter_tsv.h"

typedef void (*TOTP_CLI_LIST_HEADER_FORMATTER)();
typedef void (*TOTP_CLI_LIST_FOOTER_FORMATTER)();
typedef void (*TOTP_CLI_LIST_BODY_ITEM_FORMATTER)(size_t index, const TokenInfo* token_info);

typedef struct {
    const TOTP_CLI_LIST_HEADER_FORMATTER header_formatter;
    const TOTP_CLI_LIST_FOOTER_FORMATTER footer_formatter;
    const TOTP_CLI_LIST_BODY_ITEM_FORMATTER body_item_formatter;
} TotpCliListFormatter;

static const TotpCliListFormatter available_formatters[] = {
    {.header_formatter = &list_output_formatter_print_header_table,
     .body_item_formatter = &list_output_formatter_print_body_item_table,
     .footer_formatter = &list_output_formatter_print_footer_table},

    {.header_formatter = &list_output_formatter_print_header_tsv,
     .body_item_formatter = &list_output_formatter_print_body_item_tsv,
     .footer_formatter = &list_output_formatter_print_footer_tsv}};

#ifdef TOTP_CLI_RICH_HELP_ENABLED
void totp_cli_command_list_docopt_commands() {
    TOTP_CLI_PRINTF("  " TOTP_CLI_COMMAND_LIST ", " TOTP_CLI_COMMAND_LIST_ALT
                    "         List all available tokens\r\n");
}

void totp_cli_command_list_docopt_usage() {
    TOTP_CLI_PRINTF("  " TOTP_CLI_COMMAND_NAME " " DOCOPT_REQUIRED(
        TOTP_CLI_COMMAND_LIST " | " TOTP_CLI_COMMAND_LIST_ALT) "\r\n");
}
#endif

void totp_cli_command_list_handle(PluginState* plugin_state, FuriString* args, Cli* cli) {
    if(!totp_cli_ensure_authenticated(plugin_state, cli)) {
        return;
    }

    TokenInfoIteratorContext* iterator_context =
        totp_config_get_token_iterator_context(plugin_state);
    size_t total_count = totp_token_info_iterator_get_total_count(iterator_context);
    if(total_count <= 0) {
        TOTP_CLI_PRINTF("There are no tokens");
        return;
    }

    const TotpCliListFormatter* formatter = &available_formatters[0];
    FuriString* arg = furi_string_alloc();
    if(args_read_string_and_trim(args, arg) && furi_string_cmpi_str(arg, "--tsv") == 0) {
        formatter = &available_formatters[1];
    }

    furi_string_free(arg);

    TOTP_CLI_LOCK_UI(plugin_state);

    size_t original_index = totp_token_info_iterator_get_current_token_index(iterator_context);

    (*formatter->header_formatter)();
    for(size_t i = 0; i < total_count; i++) {
        totp_token_info_iterator_go_to(iterator_context, i);
        const TokenInfo* token_info = totp_token_info_iterator_get_current_token(iterator_context);
        (*formatter->body_item_formatter)(i, token_info);
    }

    (*formatter->footer_formatter)();

    totp_token_info_iterator_go_to(iterator_context, original_index);

    TOTP_CLI_UNLOCK_UI(plugin_state);
}