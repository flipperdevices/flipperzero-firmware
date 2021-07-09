#pragma once
#include <furi.h>
#include <filesystem-api-v2.h>
#include "storage-glue.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STORAGE_COUNT (ST_INT + 1)

typedef struct {
    FS_Api external_api;
    osMessageQueueId_t message_queue;
    StorageData storage[STORAGE_COUNT];
    StorageFileArray_t files;
} StorageApp;

#ifdef __cplusplus
}
#endif