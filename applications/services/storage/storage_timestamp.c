#include "storage_timestamp.h"
#include <m-dict.h>
#include <furi_hal.h>

#define TAG "StorageTimestamp"

DICT_DEF2(TimestampDict, uint32_t, uint32_t)

struct StorageTimestamp {
    TimestampDict_t store;
};

uint32_t storage_timestamp_hash(const char* path) {
    furi_assert(path);
    furi_assert(path[0] == '/');

    // Skipping VFS part
    path++;
    path = strstr(path, "/");

    // Extract top folder
    const char* found = NULL;
    if(path) {
        path++;
        found = strstr(path, "/");
    }

    if(found) {
        size_t len = found - path;
        uint32_t hash = m_core_hash(path, len);
        return hash;
    } else {
        return 0;
    }
}

StorageTimestamp* storage_timestamp_alloc() {
    StorageTimestamp* instance = malloc(sizeof(StorageTimestamp));
    TimestampDict_init(instance->store);
    storage_timestamp_set_by_hash(instance, 0);
    return instance;
}

void storage_timestamp_free(StorageTimestamp* instance) {
    furi_assert(instance);
    TimestampDict_clear(instance->store);
    free(instance);
}

void storage_timestamp_reset(StorageTimestamp* instance) {
    furi_assert(instance);
    TimestampDict_reset(instance->store);
    storage_timestamp_set_by_hash(instance, 0);
}

void storage_timestamp_set_by_path(StorageTimestamp* instance, const char* path) {
    storage_timestamp_set_by_hash(instance, storage_timestamp_hash(path));
}

void storage_timestamp_set_by_hash(StorageTimestamp* instance, uint32_t hash) {
    furi_assert(instance);
    uint32_t timestamp = furi_hal_rtc_get_timestamp();
    if(hash) {
        TimestampDict_set_at(instance->store, hash, timestamp);
    }
    TimestampDict_set_at(instance->store, 0, timestamp);
}

uint32_t storage_timestamp_get_by_path(StorageTimestamp* instance, const char* path) {
    return storage_timestamp_get_by_hash(instance, storage_timestamp_hash(path));
}

uint32_t storage_timestamp_get_by_hash(StorageTimestamp* instance, uint32_t hash) {
    furi_assert(instance);
    uint32_t* value = TimestampDict_get(instance->store, hash);
    return (value) ? (*value) : 0;
}

void storage_timestamp_erase(StorageTimestamp* instance, uint32_t hash) {
    furi_assert(instance);
    (void)TimestampDict_erase(instance->store, hash);
}
