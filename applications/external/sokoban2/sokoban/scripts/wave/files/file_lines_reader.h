#include "buffered_reader.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct FileLinesReader FileLinesReader;

FileLinesReader* file_lines_reader_alloc(File* file, int bufferSize);
void file_lines_reader_free(FileLinesReader* reader);

bool file_lines_reader_readln(FileLinesReader* reader, char* output, uint16_t size);
bool file_lines_reader_is_eof(FileLinesReader* reader);