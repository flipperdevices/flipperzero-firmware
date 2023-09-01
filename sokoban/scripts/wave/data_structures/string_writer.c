#include "string_writer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct StringWriter
{
    char* buffer;
    int length;
    int capacity;
};

StringWriter* string_writer_alloc(int maxCapacity)
{
    StringWriter* writer = (StringWriter*)malloc(sizeof(StringWriter));
    writer->capacity = maxCapacity;
    writer->length = 0;
    writer->buffer = (char*)malloc(maxCapacity * sizeof(char));
    memset(writer->buffer, 0, maxCapacity);
    return writer;
}

void string_writer_free(StringWriter* writer)
{
    free(writer->buffer);
    free(writer);
}

void string_writer_add_str(StringWriter* writer, char* str)
{
    int str_len = strlen(str);
    if (writer->length + str_len < writer->capacity)
    {
        memcpy(writer->buffer + writer->length, str, str_len);
        writer->length += str_len;
    }
}

void string_writer_add_str_format(StringWriter* writer, char* format, char* str)
{
    char temp[200];
    snprintf(temp, sizeof(temp), format, str);
    string_writer_add_str(writer, temp);
}

void string_writer_add_int(StringWriter* writer, int toAdd)
{
    char temp[12];
    snprintf(temp, sizeof(temp), "%d", toAdd);
    string_writer_add_str(writer, temp);
}

void string_writer_add_chars(StringWriter* writer, char* chars)
{
    string_writer_add_str(writer, chars);
}

void string_writer_clear(StringWriter* writer)
{
    memset(writer->buffer, 0, writer->length);
    writer->length = 0;
}

char* string_writer_get(StringWriter* writer)
{
    return writer->buffer;
}
