#include <stdlib.h>
#include <string.h>

typedef struct StringWriter StringWriter;

StringWriter* string_writer_alloc(int maxCapacity);
void string_writer_free(StringWriter* writer);
void string_writer_add_str(StringWriter* writer, char* str);
void string_writer_add_str_format(StringWriter* writer, char* format, char* str);
void string_writer_add_int(StringWriter* writer, int toAdd);
void string_writer_add_chars(StringWriter* writer, char* chars);
void string_writer_clear(StringWriter* writer);
char* string_writer_get(StringWriter* writer);
