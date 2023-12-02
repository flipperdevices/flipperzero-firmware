#include "list_output_formatter_tsv.h"
#include <inttypes.h>
#include "../../../../cli_helpers.h"

void list_output_formatter_print_header_tsv() {
    TOTP_CLI_PRINTF("%s\t%s\t%s\t%s\t%s\r\n", "#", "Name", "Algo", "Ln", "Type");
}

void list_output_formatter_print_body_item_tsv(size_t index, const TokenInfo* token_info) {
    TOTP_CLI_PRINTF(
        "%" PRIu16 "\t%s\t%s\t%" PRIu8 "\t%s\r\n",
        index + 1,
        furi_string_get_cstr(token_info->name),
        token_info_get_algo_as_cstr(token_info),
        token_info->digits,
        token_info_get_type_as_cstr(token_info));
}

void list_output_formatter_print_footer_tsv() {
}
