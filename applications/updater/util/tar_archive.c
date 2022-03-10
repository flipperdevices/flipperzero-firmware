#include "tar_archive.h"

#include <microtar.h>
#include <storage/storage.h>
#include <furi.h>

#define TAG "TarArch"

typedef struct TarArchive {
    Storage* storage;
    mtar_t tar;
    //mtar_header_t tar_header;
    //bool finalized;
} TarArchive;

/* API WRAPPER */
static int mtar_storage_file_write(void* stream, const void* data, unsigned size) {
    uint16_t bytes_written = storage_file_write(stream, data, size);
    return (bytes_written == size) ? bytes_written : MTAR_EWRITEFAIL;
}

static int mtar_storage_file_read(void* stream, void* data, unsigned size) {
    uint16_t bytes_read = storage_file_read(stream, data, size);
    return (bytes_read == size) ? bytes_read : MTAR_EREADFAIL;
}

static int mtar_storage_file_seek(void* stream, unsigned offset) {
    bool res = storage_file_seek(stream, offset, true);
    return res ? MTAR_ESUCCESS : MTAR_ESEEKFAIL;
}

static int mtar_storage_file_close(void* stream) {
    storage_file_close(stream);
    return MTAR_ESUCCESS;
}

const struct mtar_ops filesystem_ops = {
    .read = mtar_storage_file_read,
    .write = mtar_storage_file_write,
    .seek = mtar_storage_file_seek,
    .close = mtar_storage_file_close,
};

TarArchive* tar_archive_alloc() {
    TarArchive* archive = malloc(sizeof(TarArchive));
    archive->storage = furi_record_open("storage");
    return archive;
}

bool tar_archive_open(TarArchive* archive, const char* path, char mode) {
    furi_assert(archive);
    FS_AccessMode access_mode;
    FS_OpenMode open_mode;
    int mtar_access = 0;

    switch(mode) {
    case 'r':
        mtar_access = MTAR_READ;
        access_mode = FSAM_READ;
        open_mode = FSOM_OPEN_EXISTING;
        break;
    case 'w':
        mtar_access = MTAR_WRITE;
        access_mode = FSAM_WRITE;
        open_mode = FSOM_CREATE_ALWAYS;
        break;
    default:
        return false;
    }

    File* stream = storage_file_alloc(archive->storage);
    if(!storage_file_open(stream, path, access_mode, open_mode)) {
        storage_file_free(stream);
        return false;
    }
    mtar_init(&archive->tar, mtar_access, &filesystem_ops, stream);

    return true;
}

void tar_archive_free(TarArchive* archive) {
    furi_assert(archive);
    mtar_close(&archive->tar);
    //storage_file_free(archive->tar.stream);
    furi_record_close("storage");
}

bool tar_archive_finalize(TarArchive* archive) {
    furi_assert(archive);
    return (mtar_finalize(&archive->tar) == MTAR_ESUCCESS);
}

bool tar_archive_store_data(
    TarArchive* archive,
    const char* path,
    const uint8_t* data,
    const int32_t data_len) {
    furi_assert(archive);

    return (
        tar_archive_file_add_header(archive, path, data_len) &&
        tar_archive_file_add_data_block(archive, data, data_len) &&
        tar_archive_file_finalize(archive));
}

bool tar_archive_file_add_header(TarArchive* archive, const char* path, const int32_t data_len) {
    furi_assert(archive);

    return (mtar_write_file_header(&archive->tar, path, data_len) == MTAR_ESUCCESS);
}

bool tar_archive_file_add_data_block(
    TarArchive* archive,
    const uint8_t* data_block,
    const int32_t block_len) {
    furi_assert(archive);

    return (mtar_write_data(&archive->tar, data_block, block_len) == block_len);
}

bool tar_archive_file_finalize(TarArchive* archive) {
    furi_assert(archive);
    return (mtar_end_data(&archive->tar) == MTAR_ESUCCESS);
}

typedef struct {
    TarArchive* archive;
    const char* work_dir;
} TarArchiveDirectoryOpParams;

static int archive_extract_foreach_cb(mtar_t* tar, const mtar_header_t* header, void* param) {
    TarArchiveDirectoryOpParams* op_params = param;

    const int TAR_READ_BUF_SZ = 512;

    if(header->type != MTAR_TREG) {
        fprintf(stderr, "warning: not extracting unsupported type \"%s\"", header->name);
        return 0;
    }
    string_t fname;
    //string_init(fname);

    string_init_printf(fname, "%s/%s", op_params->work_dir, header->name);
    FURI_LOG_I(TAG, "Extracting %d bytes to '%s'", header->size, header->name);
    File* out_file = storage_file_alloc(op_params->archive->storage);
    uint8_t* readbuf = malloc(TAR_READ_BUF_SZ);

    bool failed = false;
    do {
        if(!storage_file_open(out_file, string_get_cstr(fname), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
            failed = true;
            break;
        }

        while(!mtar_eof_data(tar)) {
            int32_t readcnt = mtar_read_data(tar, readbuf, TAR_READ_BUF_SZ);
            if(!readcnt || !storage_file_write(out_file, readbuf, readcnt)) {
                failed = true;
                break;
            }
        }
    } while(false);

    storage_file_free(out_file);
    free(readbuf);
    string_clear(fname);
    return failed ? -1 : 0;
}

bool tar_archive_unpack_to(TarArchive* archive, const char* destination) {
    furi_assert(archive);
    TarArchiveDirectoryOpParams param = {
        .archive = archive,
        .work_dir = destination,
    };
    return (mtar_foreach(&archive->tar, archive_extract_foreach_cb, &param) == MTAR_ESUCCESS);
};
