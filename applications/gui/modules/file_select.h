#pragma once
#include <gui/view.h>
#include <filesystem-api.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FileSelect FileSelect;

typedef void (*FileSelectCallback)(char* result, void* context);

FileSelect* file_select_alloc();

void file_select_free(FileSelect* file_select);

void file_select_set_api(FileSelect* file_select, FS_Api* fs_api);
void file_select_set_callback(FileSelect* file_select, FS_Api* fs_api);
void file_select_set_filter(FileSelect* file_select, char* path, char* extension);

#ifdef __cplusplus
}
#endif