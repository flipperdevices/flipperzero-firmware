#pragma once

#include <furi.h>

typedef struct StorageTimestamp StorageTimestamp;

uint32_t storage_timestamp_hash(const char* path);

StorageTimestamp* storage_timestamp_alloc();

void storage_timestamp_free(StorageTimestamp* instance);

void storage_timestamp_reset(StorageTimestamp* instance);

void storage_timestamp_set_by_path(StorageTimestamp* instance, const char* path);

void storage_timestamp_set_by_hash(StorageTimestamp* instance, uint32_t hash);

uint32_t storage_timestamp_get_by_path(StorageTimestamp* instance, const char* path);

uint32_t storage_timestamp_get_by_hash(StorageTimestamp* instance, uint32_t hash);

void storage_timestamp_remove(StorageTimestamp* instance, uint32_t hash);
