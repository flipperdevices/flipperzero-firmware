#include "buffered_reader.h"

typedef struct BufferedReader
{
    File* file;
    char* buffer;
    int bufferSize;
    int bufferPos;
    int bufferLen;
} BufferedReader;

void buffered_reader_free(BufferedReader* reader)
{
    free(reader->buffer);
    free(reader);
}

BufferedReader* bufferedReader_alloc(File* file, int bufferSize)
{
    furi_assert(storage_file_is_open(file));

    BufferedReader* reader = malloc(sizeof(BufferedReader));
    reader->file = file;
    reader->buffer = malloc(bufferSize);
    reader->bufferSize = bufferSize;
    reader->buffer[0] = '\0';
    reader->bufferPos = 0;
    reader->bufferLen = 0;
    return reader;
}

static void buffered_reader_fill_buffer(BufferedReader* reader)
{
    if (reader->bufferPos < reader->bufferLen)
        return;

    reader->bufferPos = 0;
    reader->bufferLen = storage_file_read(reader->file, reader->buffer, reader->bufferSize);
}

bool buffered_reader_is_eof(BufferedReader* reader)
{
    buffered_reader_fill_buffer(reader);
    return reader->bufferPos >= reader->bufferLen;
}

char buffered_reader_read_char(BufferedReader* reader)
{
    buffered_reader_fill_buffer(reader);
    if (reader->bufferPos >= reader->bufferLen)
        return '\0';

    return reader->buffer[reader->bufferPos++];
}