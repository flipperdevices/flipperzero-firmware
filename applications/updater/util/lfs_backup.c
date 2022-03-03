#include "lfs_backup.h"

#include <microtar.h>
#include <storage/storage.h>
#include <furi.h>

typedef struct {
    Storage* storage;
    File* archive;
    mtar_t tar;
    mtar_header_t tar_header;
} LfsArchive;

//static LfsArchive* lfs_archive_alloc() {
//    LfsArchive* archive = malloc(sizeof(LfsArchive));
//    archive->tar;

//    return archive;
//}

bool lfs_backup_create() {
    return false;
}

bool lfs_backup_exists() {
    return false;
}

bool lfs_backup_unpack() {
    return false;
}
