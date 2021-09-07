#pragma once
#include "file-worker.h"

#define MAX_FILES 100 //temp

void archive_trim_file_ext(char* name);
bool archive_get_filenames(void* context, uint8_t tab_id, const char* path);
bool archive_read_dir(void* context, const char* path);
void archive_file_append(const char* path, string_t string);
void archive_delete_file(void* context, string_t path, string_t name);