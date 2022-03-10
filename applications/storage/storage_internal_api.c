#include <furi/record.h>
#include <m-string.h>
#include "storage.h"
#include <updater/util/tar_archive.h>

#define MAX_NAME_LEN 255
#define FILE_BLOCK_SIZE 512

static bool storage_int_backup_add_file(
    Storage* api,
    TarArchive* archive,
    const char* fname,
    const int32_t file_size) {
    uint8_t* file_buffer = malloc(FILE_BLOCK_SIZE);
    bool success = false;
    File* src_file = storage_file_alloc(api);
    string_t full_src_fname;
    string_init_printf(full_src_fname, "/int/%s", fname);
    do {
        if(!storage_file_open(
               src_file, string_get_cstr(full_src_fname), FSAM_READ, FSOM_OPEN_EXISTING) ||
           !tar_archive_file_add_header(archive, fname, file_size)) {
            break;
        }

        uint16_t bytes_read = 0;
        while((bytes_read = storage_file_read(src_file, file_buffer, FILE_BLOCK_SIZE))) {
            success = tar_archive_file_add_data_block(archive, file_buffer, bytes_read);
            if(!success) {
                break;
            }
        }

        success = success && tar_archive_file_finalize(archive);
    } while(false);

    string_clear(full_src_fname);
    storage_file_free(src_file);
    free(file_buffer);
    return success;
}

FS_Error storage_int_backup(Storage* api, const char* dstname) {
    TarArchive* archive = tar_archive_alloc();
    File* directory = storage_file_alloc(api);
    FileInfo file_info;
    char name[MAX_NAME_LEN];
    bool success = false;
    do {
        if(!tar_archive_open(archive, dstname, 'w') || !storage_dir_open(directory, "/int")) {
            break;
        }

        while(true) {
            if(!storage_dir_read(directory, &file_info, name, sizeof(name))) {
                break;
            }

            success = storage_int_backup_add_file(api, archive, name, file_info.size);
            if(!success) {
                break;
            }
        }

        success = success && tar_archive_finalize(archive);
    } while(false);

    storage_file_free(directory);
    tar_archive_free(archive);
    return success ? FSE_OK : FSE_INTERNAL;
}

FS_Error storage_int_restore(Storage* api, const char* dstname) {
    TarArchive* archive = tar_archive_alloc();

    bool success = tar_archive_open(archive, dstname, 'r') &&
                   tar_archive_unpack_to(archive, "/int");

    tar_archive_free(archive);
    return success ? FSE_OK : FSE_INTERNAL;
}