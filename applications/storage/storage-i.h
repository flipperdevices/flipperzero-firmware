#pragma once
#include <furi.h>
#include <filesystem-api-v2.h>
#include "storage-glue.h"
#include <gui/gui.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STORAGE_COUNT (ST_INT + 1)

typedef struct {
    ViewPort* view_port;
    Icon* icon_mounted;
    Icon* icon_error;
    bool enabled;
} StorageAppSDGui;

typedef struct {
    FS_Api external_api;
    osMessageQueueId_t message_queue;
    StorageData storage[STORAGE_COUNT];
    StorageAppSDGui sd_gui;
} StorageApp;

#ifdef __cplusplus
}
#endif