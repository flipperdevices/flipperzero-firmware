#pragma once
#include <furi.h>
#include <filesystem-api.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    FS_Api external_api;
    osMessageQueueId_t message_queue;
} FilesystemApp;

#ifdef __cplusplus
}
#endif