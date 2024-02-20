#include "uf2.h"

#include <furi.h>

#define UF2_BLOCK_SIZE (512UL)
#define UF2_DATA_SIZE (476UL)
#define UF2_CHECKSUM_SIZE (16UL)

#define UF2_MAGIC_START_0 (0x0A324655UL)
#define UF2_MAGIC_START_1 (0x9E5D5157UL)
#define UF2_MAGIC_END (0x0AB16F30UL)

#define TAG "VgmUf2"

typedef enum {
    Uf2FlagNotMainFlash = 1UL << 0,
    Uf2FlagFileContainer = 1UL << 12,
    Uf2FlagFamilyIdPresent = 1UL << 13,
    Uf2FlagChecksumPresent = 1UL << 14,
    Uf2FlagExtensionPresent = 1UL << 15,
} Uf2Flag;

typedef struct {
    uint32_t magic_start[2];
    uint32_t flags;
    uint32_t target_addr;
    uint32_t payload_size;
    uint32_t block_no;
    uint32_t num_blocks;
    union {
        uint32_t file_size;
        uint32_t family_id;
    };
} Uf2BlockHeader;

typedef union {
    uint8_t payload[UF2_DATA_SIZE];
    struct {
        uint8_t reserved[UF2_DATA_SIZE - 24];
        uint32_t start_addr;
        uint32_t region_len;
        uint8_t checksum[UF2_CHECKSUM_SIZE];
    };
} Uf2BlockData;

typedef struct {
    uint32_t magic_end;
} Uf2BlockTrailer;

static bool uf2_block_header_read(Uf2BlockHeader* header, File* file) {
    const size_t size_read = storage_file_read(file, header, sizeof(Uf2BlockHeader));
    return size_read == sizeof(Uf2BlockHeader);
}

static bool
    uf2_block_header_verify(const Uf2BlockHeader* header, uint32_t family_id, size_t payload_size) {
    bool success = false;

    do {
        if(header->magic_start[0] != UF2_MAGIC_START_0) break;
        if(header->magic_start[1] != UF2_MAGIC_START_1) break;
        if(header->flags & Uf2FlagNotMainFlash) {
            FURI_LOG_E(TAG, "Non-flash blocks are not supported (block #%lu)", header->block_no);
            break;
        }
        if(header->flags & Uf2FlagFamilyIdPresent) {
            if(header->family_id != family_id) {
                FURI_LOG_E(
                    TAG,
                    "Family ID expected: %lX, got: %lX (block #%lu)",
                    family_id,
                    header->family_id,
                    header->block_no);
                break;
            }
        }
        if(header->payload_size != payload_size) {
            FURI_LOG_E(
                TAG,
                "Only %zu-byte block payloads are supported (block #%lu)",
                payload_size,
                header->block_no);
            break;
        }
        if(header->target_addr % payload_size != 0) {
            FURI_LOG_E(
                TAG,
                "Only %zu-byte aligned  are allowed (block #%lu)",
                payload_size,
                header->block_no);
            break;
        }
        success = true;
    } while(false);

    return success;
}

static bool uf2_block_header_skip(File* file) {
    return storage_file_seek(file, sizeof(Uf2BlockHeader), false);
}

static bool uf2_block_payload_skip(File* file) {
    return storage_file_seek(file, sizeof(Uf2BlockData), false);
}

static bool uf2_block_trailer_skip(File* file) {
    return storage_file_seek(file, sizeof(Uf2BlockTrailer), false);
}

static bool uf2_block_payload_read(File* file, void* payload_data, size_t payload_size) {
    bool success = false;

    do {
        const size_t size_read = storage_file_read(file, payload_data, payload_size);
        if(size_read != payload_size) break;
        if(!storage_file_seek(file, UF2_DATA_SIZE - payload_size, false)) break;
        success = true;
    } while(false);

    return success;
}

static bool uf2_block_trailer_read(Uf2BlockTrailer* trailer, File* file) {
    const size_t size_read = storage_file_read(file, trailer, sizeof(Uf2BlockTrailer));
    return size_read == sizeof(Uf2BlockTrailer);
}

static bool uf2_block_trailer_verify(const Uf2BlockTrailer* trailer) {
    return trailer->magic_end == UF2_MAGIC_END;
}

bool uf2_get_block_count(File* file, uint32_t* block_count) {
    const size_t file_size = storage_file_size(file);

    if(file_size == 0) {
        FURI_LOG_E(TAG, "File size is zero");
        return false;
    } else if(file_size % UF2_BLOCK_SIZE != 0) {
        FURI_LOG_E(TAG, "File size is not a multiple of %lu bytes", UF2_BLOCK_SIZE);
        return false;
    }

    *block_count = file_size / UF2_BLOCK_SIZE;
    return true;
}

bool uf2_verify_block(File* file, uint32_t family_id, size_t payload_size) {
    Uf2BlockHeader header;
    Uf2BlockTrailer trailer;

    if(!uf2_block_header_read(&header, file)) return false;
    if(!uf2_block_header_verify(&header, family_id, payload_size)) return false;
    if(!uf2_block_payload_skip(file)) return false;
    if(!uf2_block_trailer_read(&trailer, file)) return false;
    if(!uf2_block_trailer_verify(&trailer)) return false;

    return true;
}

bool uf2_read_block(File* file, void* payload_data, size_t payload_size) {
    if(!uf2_block_header_skip(file)) return false;
    if(!uf2_block_payload_read(file, payload_data, payload_size)) return false;
    if(!uf2_block_trailer_skip(file)) return false;

    return true;
}
