#pragma once
#include <furi.h>
#include <gui/gui.h>
#include "storage-glue.h"
#include "storage-sd-api.h"
#include "filesystem-api-internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STORAGE_COUNT (ST_INT + 1)

typedef struct {
    ViewPort* view_port;
    bool enabled;
} StorageAppSDGui;

struct StorageApp {
    osMessageQueueId_t message_queue;
    StorageData storage[STORAGE_COUNT];
    StorageAppSDGui sd_gui;
};

#ifdef __cplusplus
}
#endif