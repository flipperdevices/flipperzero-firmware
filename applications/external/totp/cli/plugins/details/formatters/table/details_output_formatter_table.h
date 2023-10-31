#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void details_output_formatter_print_header_table();
void details_output_formatter_print_footer_table();
void details_output_formatter_print_automation_feature_table(
    const char* key,
    const char* feature,
    bool* header_printed);
void details_output_formatter_print_cstr_table(const char* key, const char* value);
void details_output_formatter_print_uint8t_table(const char* key, uint8_t value);
void details_output_formatter_print_sizet_table(const char* key, size_t value);
void details_output_formatter_print_uint64t_table(const char* key, uint64_t value);
