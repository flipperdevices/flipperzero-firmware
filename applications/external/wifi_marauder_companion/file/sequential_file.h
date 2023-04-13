#pragma once

#include <storage/storage.h>

typedef struct SequentialFile {
    File* file;
    char* path;
    int file_index;
} SequentialFile;

char* sequential_file_resolve_path(Storage* storage, const char* dir, const char* prefix, const char* extension);
SequentialFile* sequential_file_create(Storage* storage, const char* dir, const char* prefix, const char* extension);
uint16_t sequential_file_write(SequentialFile* file, const void *buff, uint16_t bytes_to_write);
void sequential_file_free_destroy_file(SequentialFile* file);
void sequential_file_free_close_file(SequentialFile* file);