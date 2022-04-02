#include "lfs_backup.h"

#include "tar_archive.h"
#include <storage/storage.h>

const char* DEFAULT_BACKUP_FILENAME = "backup.tar";
const char* DEFAULT_BACKUP_LOCATION = "/ext/backup.tar";

bool lfs_backup_create(Storage* storage, const char* destination) {
    const char* final_destination = destination && strlen(destination) ? destination :
                                                                         DEFAULT_BACKUP_LOCATION;
    return storage_int_backup(storage, final_destination) == FSE_OK;
}

bool lfs_backup_exists(Storage* storage, const char* source) {
    const char* final_source = source && strlen(source) ? source : DEFAULT_BACKUP_LOCATION;
    FileInfo fi;
    return storage_common_stat(storage, final_source, &fi) == FSE_OK;
}

bool lfs_backup_unpack(Storage* storage, const char* source) {
    const char* final_source = source && strlen(source) ? source : DEFAULT_BACKUP_LOCATION;
    return storage_int_restore(storage, final_source) == FSE_OK;
}
