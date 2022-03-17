#include "lfs_backup.h"

#include "tar_archive.h"
#include <storage/storage.h>

#define BACKUP_FILE_LOCATION "/ext/backup.tar"

bool lfs_backup_create() {
    Storage* storage = furi_record_open("storage");
    bool success = storage_int_backup(storage, BACKUP_FILE_LOCATION) == FSE_OK;
    furi_record_close("storage");
    return success;
}

bool lfs_backup_exists() {
    FileInfo fi;
    Storage* storage = furi_record_open("storage");
    bool success = storage_common_stat(storage, BACKUP_FILE_LOCATION, &fi) == FSE_OK;
    furi_record_close("storage");
    return success;
}

bool lfs_backup_unpack() {
    Storage* storage = furi_record_open("storage");
    bool success = storage_int_restore(storage, BACKUP_FILE_LOCATION) == FSE_OK;
    furi_record_close("storage");
    return success;
}
