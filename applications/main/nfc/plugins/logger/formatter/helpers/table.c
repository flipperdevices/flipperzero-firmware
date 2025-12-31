#include "table.h"

typedef struct {
    char column;
    char row;
} TableDelimeterSymbols;

struct Table {
    size_t column_count;
    size_t* columns_width;
    TableColumnDataAlignment* aligns;

    FuriString** names;
    FuriString* format;

    TableDelimeterSymbols delimeters_header;
    TableDelimeterSymbols delimeters_data;
};

static void table_alloc_names_array(Table* table, const char** names) {
    size_t names_furi_string_array_size_bytes = sizeof(FuriString*) * table->column_count;
    table->names = malloc(names_furi_string_array_size_bytes);
    for(size_t i = 0; i < table->column_count; i++) {
        table->names[i] = furi_string_alloc_set_str(names[i]);
    }
}

static void table_free_names_array(Table* table) {
    for(size_t i = 0; i < table->column_count; i++) {
        furi_string_free(table->names[i]);
    }
    free(table->names);
}

static void table_alloc_width_array(Table* table, const size_t* columns_width) {
    size_t width_array_size_bytes = sizeof(size_t) * table->column_count;
    table->columns_width = malloc(width_array_size_bytes);
    memcpy(table->columns_width, columns_width, width_array_size_bytes);
}

static void table_generate_format_string(
    FuriString* format_output,
    const size_t* columns_width,
    const size_t column_count,
    const TableColumnDataAlignment* align,
    const char column_delimeter) {
    furi_string_reset(format_output);
    for(size_t i = 0; i < column_count; i++) {
        furi_string_cat_printf(
            format_output,
            align[i] == TableColumnDataAlignmentLeft ? "%c%%-%ds" : "%c%%%ds",
            column_delimeter,
            columns_width[i]);
    }
    furi_string_cat_printf(format_output, "%c\r\n", column_delimeter);
}

Table* table_alloc(size_t column_count, const size_t* columns_width, const char** names) {
    furi_assert(column_count > 0);
    furi_assert(columns_width);
    furi_assert(names);

    Table* table = malloc(sizeof(Table));

    table->column_count = column_count;
    table->delimeters_header.column = '+';
    table->delimeters_header.row = '-';
    table->delimeters_data.column = '|';

    table->aligns = malloc(column_count);
    memset(table->aligns, TableColumnDataAlignmentRight, column_count);

    table_alloc_width_array(table, columns_width);
    table_alloc_names_array(table, names);

    table->format = furi_string_alloc();
    table_generate_format_string(
        table->format,
        table->columns_width,
        table->column_count,
        table->aligns,
        table->delimeters_data.column);
    return table;
}

void table_free(Table* table) {
    furi_assert(table);

    furi_string_free(table->format);
    table_free_names_array(table);
    free(table->columns_width);
    free(table->aligns);
    free(table);
}

void table_set_column_alignment(
    Table* table,
    size_t column_index,
    TableColumnDataAlignment alignment) {
    furi_assert(table);
    furi_assert(column_index < table->column_count);
    furi_assert(alignment < TableColumnDataAlignmentNum);
    table->aligns[column_index] = alignment;
    table_generate_format_string(
        table->format,
        table->columns_width,
        table->column_count,
        table->aligns,
        table->delimeters_data.column);
}

static void table_get_column_format(Table* table, uint8_t column_index, FuriString* output) {
    furi_assert(column_index < table->column_count);

    const char column_delimeter = table->delimeters_data.column;
    const size_t* columns_width = table->columns_width;
    if(table->aligns[column_index] == TableColumnDataAlignmentRight) {
        furi_string_printf(
            output,
            (column_index < table->column_count - 1) ? "%c%%%ds" : "%c%%%ds%c\r\n",
            column_delimeter,
            columns_width[column_index],
            column_delimeter);
    } else {
        furi_string_printf(
            output,
            (column_index < table->column_count - 1) ? "%c%%-%ds" : "%c%%-%ds%c\r\n",
            column_delimeter,
            columns_width[column_index],
            column_delimeter);
    }
}

void table_printf_header(Table* table, FuriString* output) {
    furi_assert(table);
    furi_assert(output);
    FuriString* header_format = furi_string_alloc();

    for(size_t i = 0; i < table->column_count; i++) {
        table_get_column_format(table, i, header_format);
        furi_string_cat_printf(
            output, furi_string_get_cstr(header_format), furi_string_get_cstr(table->names[i]));
    }

    for(size_t i = 0; i < table->column_count; i++) {
        furi_string_cat_printf(output, "%c", table->delimeters_header.column);
        for(size_t j = 0; j < table->columns_width[i]; j++) {
            furi_string_cat_printf(output, "%c", table->delimeters_header.row);
        }
    }
    furi_string_cat_printf(output, "%c\r\n", table->delimeters_header.column);

    furi_string_free(header_format);
}

void table_printf_row(Table* table, FuriString* output, ...) {
    va_list args;
    va_start(args, output);

    furi_string_cat_vprintf(output, furi_string_get_cstr(table->format), args);
    va_end(args);
}

void table_printf_row_array(Table* table, FuriString* output, FuriString** data, size_t data_count) {
    furi_assert(table);
    furi_assert(output);
    furi_assert(data);
    furi_assert(data_count == table->column_count);

    const size_t* columns_width = table->columns_width;
    FuriString* format = furi_string_alloc();
    FuriString* split_buffer = furi_string_alloc();

    size_t* split_indexes = malloc(sizeof(size_t) * table->column_count);

    uint8_t lines_per_row = 1;
    for(uint8_t line = 0; line < lines_per_row; line++) {
        for(size_t i = 0; i < table->column_count; i++) {
            const size_t col_width = columns_width[i];
            size_t line_length = furi_string_size(data[i]);

            uint8_t lines_per_data = line_length / col_width;
            if(line_length % col_width != 0) {
                lines_per_data += 1;
            }

            lines_per_row = MAX(lines_per_row, lines_per_data);

            if(split_indexes[i] == line_length) {
                furi_string_set_str(split_buffer, "");
            } else {
                size_t offset_len = (line_length - split_indexes[i]) > col_width ?
                                        col_width :
                                        (line_length - split_indexes[i]);

                furi_string_set_n(split_buffer, data[i], split_indexes[i], offset_len);
                split_indexes[i] += offset_len;
            }

            table_get_column_format(table, i, format);
            furi_string_cat_printf(
                output, furi_string_get_cstr(format), furi_string_get_cstr(split_buffer));
        }
    }

    free(split_indexes);
    furi_string_free(format);
    furi_string_free(split_buffer);
}

void table_append_formatted_row(Table* table, FuriString* formatted_row, FuriString* output) {
    furi_assert(table);
    furi_assert(formatted_row);
    furi_string_cat(output, formatted_row);
}

const char* table_get_format_string(Table* table) {
    furi_assert(table);
    return furi_string_get_cstr(table->format);
}
