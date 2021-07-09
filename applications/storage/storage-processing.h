#pragma once
#include <furi.h>
#include "storage-i.h"
#include "storage-message.h"
#include "storage-glue.h"

#ifdef __cplusplus
extern "C" {
#endif

void storage_process_message(StorageApp* app, StorageMessage* message);

#ifdef __cplusplus
}
#endif