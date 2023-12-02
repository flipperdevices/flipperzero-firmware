#include <stdbool.h>
#include <storage/storage.h>

typedef struct BufferedReader BufferedReader;

BufferedReader* bufferedReader_alloc(File* file, int bufferSize);
void buffered_reader_free(BufferedReader* reader);

bool buffered_reader_is_eof(BufferedReader* reader);
char buffered_reader_read_char(BufferedReader* reader);