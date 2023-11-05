#include "file_lines_reader.h"

typedef struct FileLinesReader
{
    BufferedReader* buffer;
} FileLinesReader;

void file_lines_reader_free(FileLinesReader* reader)
{
    buffered_reader_free(reader->buffer);
    free(reader);
}

FileLinesReader* file_lines_reader_alloc(File* file, int bufferSize)
{
    furi_assert(storage_file_is_open(file));

    FileLinesReader* reader = malloc(sizeof(FileLinesReader));
    reader->buffer = bufferedReader_alloc(file, bufferSize);
    return reader;
}

bool file_lines_reader_readln(FileLinesReader* reader, char* output, uint16_t size)
{
    char ch;
    int outputPos;
    for (outputPos = 0; outputPos < size - 1;)
    {
        if (buffered_reader_is_eof(reader->buffer))
            break;

        ch = buffered_reader_read_char(reader->buffer);
        if (ch == '\0')
            break;

        if (ch == '\r')
            continue;

        if (ch == '\n')
            break;

        output[outputPos++] = ch;
    }
    output[outputPos] = '\0';
    return outputPos != 0 || !buffered_reader_is_eof(reader->buffer);
}

bool file_lines_reader_is_eof(FileLinesReader* reader)
{
    return buffered_reader_is_eof(reader->buffer);
}