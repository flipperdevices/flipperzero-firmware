#pragma once
#include <furi.h>
#include <filesystem-api-v2.h>
#include <m-string.h>
#include <m-array.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { ST_EXT = 0, ST_INT = 1, ST_ANY, ST_ERROR } StorageType;

typedef struct StorageData StorageData;

typedef struct {
    void (*tick)(StorageData* storage);
} StorageApi;

typedef struct {
    File* file;
    StorageType type;
    void* file_data;
    string_t path;
} StorageFile;

void storage_file_init(StorageFile* obj);
void storage_file_init_set(StorageFile* obj, const StorageFile* src);
void storage_file_set(StorageFile* obj, const StorageFile* src);
void storage_file_clear(StorageFile* obj);

void storage_data_init(StorageData* storage);
bool storage_data_lock(StorageData* storage);
bool storage_data_unlock(StorageData* storage);
FS_Error storage_data_status(StorageData* storage);

ARRAY_DEF(
    StorageFileArray,
    StorageFile,
    (INIT(API_2(storage_file_init)),
     SET(API_6(storage_file_init_set)),
     INIT_SET(API_6(storage_file_set)),
     CLEAR(API_2(storage_file_clear))))

struct StorageData {
    FS_Api fs_api;
    StorageApi api;
    void* data;
    osMutexId_t mutex;
    FS_Error status;
    StorageFileArray_t files;
};

StorageType storage_get_type_by_file(const File* file, StorageFileArray_t array);
StorageType storage_get_type_by_path(const char* path);
bool storage_path_already_open(const char* path, StorageFileArray_t array);

void storage_set_storage_file_data(const File* file, void* file_data, StorageData* storage);
void* storage_get_storage_file_data(const File* file, StorageData* storage);

void storage_push_storage_file(
    File* file,
    const char* path,
    StorageType type,
    StorageData* storage);

#ifdef __cplusplus
}
#endif