#include "list_output_formatter_table.h"
#include <inttypes.h>
#include "../../../../cli_helpers.h"

void list_output_formatter_print_header_table() {
    TOTP_CLI_PRINTF("+-----+---------------------------+--------+----+------+\r\n");
    TOTP_CLI_PRINTF("| %-3s | %-25s | %-6s | %-s | %-s |\r\n", "#", "Name", "Algo", "Ln", "Type");
    TOTP_CLI_PRINTF("+-----+---------------------------+--------+----+------+\r\n");
}

void list_output_formatter_print_body_item_table(size_t index, const TokenInfo* token_info) {
    TOTP_CLI_PRINTF(
        "| %-3" PRIu16 " | %-25.25s | %-6s | %-2" PRIu8 " | %-4s |\r\n",
        index + 1,
        furi_string_get_cstr(token_info->name),
        token_info_get_algo_as_cstr(token_info),
        token_info->digits,
        token_info_get_type_as_cstr(token_info));
}

void list_output_formatter_print_footer_table() {
    TOTP_CLI_PRINTF("+-----+---------------------------+--------+----+------+\r\n");
}
