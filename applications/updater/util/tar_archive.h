#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TarArchive TarArchive;

TarArchive* tar_archive_alloc();

bool tar_archive_open(TarArchive* archive, const char* path, char mode);

void tar_archive_free(TarArchive* archive);

bool tar_archive_file_add_header(TarArchive* archive, const char* path, const int32_t data_len);

bool tar_archive_file_add_data_block(
    TarArchive* archive,
    const uint8_t* data_block,
    const int32_t block_len);

bool tar_archive_file_finalize(TarArchive* archive);

bool tar_archive_store_data(
    TarArchive* archive,
    const char* path,
    const uint8_t* data,
    const int32_t data_len);

bool tar_archive_finalize(TarArchive* archive);
bool tar_archive_unpack_to(TarArchive* archive, const char* destination);

#ifdef __cplusplus
}
#endif