#include "lfs-storage.h"

void storage_lfs_tick(StorageData* storage);

void storage_lfs_init(StorageData* storage) {
    storage->api.tick = storage_lfs_tick;
}

void storage_lfs_tick(StorageData* storage) {
}