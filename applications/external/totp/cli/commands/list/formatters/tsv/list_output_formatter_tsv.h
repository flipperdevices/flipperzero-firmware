#pragma once

#include "../../../../../types/token_info.h"

void list_output_formatter_print_header_tsv();

void list_output_formatter_print_body_item_tsv(size_t index, const TokenInfo* token_info);

void list_output_formatter_print_footer_tsv();