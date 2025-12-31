#pragma once

#include <furi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TableColumnDataAlignmentLeft,
    TableColumnDataAlignmentRight,

    TableColumnDataAlignmentNum
} TableColumnDataAlignment;

typedef struct Table Table;

Table* table_alloc(size_t column_count, const size_t* columns_width, const char** names);
void table_free(Table* table);
void table_set_column_alignment(
    Table* table,
    size_t column_index,
    TableColumnDataAlignment alignment);

const char* table_get_format_string(Table* table);
void table_append_formatted_row(Table* table, FuriString* formatted_row, FuriString* output);

void table_printf_header(Table* table, FuriString* output);
void table_printf_row(Table* table, FuriString* output, ...);
void table_printf_row_array(Table* table, FuriString* output, FuriString** data, size_t data_count);

#ifdef __cplusplus
}
#endif
