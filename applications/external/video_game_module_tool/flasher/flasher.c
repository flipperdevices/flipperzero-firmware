#include "flasher.h"

#include <furi.h>
#include <storage/storage.h>

#include "uf2.h"
#include "swd.h"
#include "board.h"
#include "target.h"
#include "rp2040.h"

#define TAG "VgmFlasher"

#define W25Q128_CAPACITY (0x1000000UL)
#define W25Q128_PAGE_SIZE (0x100UL)
#define W25Q128_SECTOR_SIZE (0x1000UL)

#define PROGRESS_VERIFY_WEIGHT (4U)
#define PROGRESS_ERASE_WEIGHT (6U)
#define PROGRESS_PROGRAM_WEIGHT (90U)

#define FLASHER_ATTEMPT_COUNT (10UL)

typedef struct {
    FlasherCallback callback;
    void* context;
} Flasher;

static Flasher flasher;

bool flasher_init(void) {
    FURI_LOG_D(TAG, "Attaching the target");

    board_init();

    bool success = false;
    FURI_CRITICAL_ENTER();
    do {
        swd_init();
        if(!target_attach(RP2040_CORE0_ADDR)) {
            FURI_LOG_E(TAG, "Failed to attach target");
            break;
        }
        success = true;
    } while(false);
    FURI_CRITICAL_EXIT();

    if(!success) {
        flasher_deinit();
    }

    return success;
}

void flasher_deinit(void) {
    FURI_LOG_D(TAG, "Detaching target and restoring pins");

    FURI_CRITICAL_ENTER();
    target_detach();
    swd_deinit();
    FURI_CRITICAL_EXIT();

    board_reset();
    board_deinit();
}

void flasher_set_callback(FlasherCallback callback, void* context) {
    flasher.callback = callback;
    flasher.context = context;
}

static inline bool flasher_init_chip(void) {
    FURI_CRITICAL_ENTER();
    const bool success = rp2040_init();
    FURI_CRITICAL_EXIT();
    return success;
}

static inline bool flasher_erase_sector(uint32_t address) {
    FURI_CRITICAL_ENTER();
    const bool success = rp2040_flash_erase_sector(address);
    FURI_CRITICAL_EXIT();
    return success;
}

static inline bool flasher_program_page(uint32_t address, const void* data, size_t data_size) {
    FURI_CRITICAL_ENTER();
    const bool success = rp2040_flash_program_page(address, data, data_size);
    FURI_CRITICAL_EXIT();
    return success;
}

static void flasher_emit_progress(uint8_t start, uint8_t weight, uint8_t progress) {
    furi_assert(flasher.callback);

    FlasherEvent event = {
        .type = FlasherEventTypeProgress,
        .progress = start + ((uint32_t)weight * progress) / 100U,
    };

    flasher.callback(event, flasher.context);
}

static void flasher_emit_error(FlasherError error) {
    furi_assert(flasher.callback);

    FlasherEvent event = {
        .type = FlasherEventTypeError,
        .error = error,
    };

    flasher.callback(event, flasher.context);
}

static void flasher_emit_success(void) {
    furi_assert(flasher.callback);

    FlasherEvent event = {
        .type = FlasherEventTypeSuccess,
    };

    flasher.callback(event, flasher.context);
}

static bool flasher_prepare_target(void) {
    bool success = false;

    for(uint32_t i = 0; i < FLASHER_ATTEMPT_COUNT; ++i) {
        if(flasher_init()) {
            success = true;
            break;
        }
        furi_delay_ms(10);
    }

    if(!success) {
        flasher_emit_error(FlasherErrorDisconnect);
    }

    return success;
}

static bool flasher_prepare_file(File* file, const char* file_path) {
    bool success = false;

    do {
        if(!flasher_init_chip()) {
            FURI_LOG_E(TAG, "Failed to initialise chip");
            flasher_emit_error(FlasherErrorDisconnect);
            break;
        }
        if(!storage_file_open(file, file_path, FSAM_READ, FSOM_OPEN_EXISTING)) {
            FURI_LOG_E(TAG, "Failed to open firmware file: %s", file_path);
            flasher_emit_error(FlasherErrorBadFile);
            break;
        }
        success = true;
    } while(false);

    return success;
}

static bool flasher_verify_file(File* file, size_t* data_size) {
    bool success = false;

    do {
        uint32_t block_count;
        if(!uf2_get_block_count(file, &block_count)) {
            FURI_LOG_E(TAG, "Failed to get block count");
            flasher_emit_error(FlasherErrorBadFile);
            break;
        }

        uint32_t blocks_verified;
        uint8_t prev_progress = UINT8_MAX;

        for(blocks_verified = 0; blocks_verified < block_count; ++blocks_verified) {
            if(!uf2_verify_block(file, RP2040_FAMILY_ID, W25Q128_PAGE_SIZE)) break;

            const uint8_t verify_progress = (blocks_verified * 100UL) / block_count;
            if(verify_progress != prev_progress) {
                prev_progress = verify_progress;
                flasher_emit_progress(0, PROGRESS_VERIFY_WEIGHT, verify_progress);
                FURI_LOG_D(TAG, "Verifying file: %u%%", verify_progress);
            }
        }

        if(blocks_verified < block_count) {
            FURI_LOG_E(TAG, "Failed to verify all blocks");
            flasher_emit_error(FlasherErrorBadFile);
            break;
        }

        const size_t size_total = block_count * W25Q128_PAGE_SIZE;

        if(size_total > W25Q128_CAPACITY) {
            FURI_LOG_E(TAG, "File is too large to fit on the flash");
            flasher_emit_error(FlasherErrorBadFile);
            break;
        }

        if(!storage_file_seek(file, 0, true)) {
            FURI_LOG_E(TAG, "Failed to rewind the file");
            flasher_emit_error(FlasherErrorBadFile);
            break;
        }

        *data_size = size_total;
        success = true;
    } while(false);

    return success;
}

static bool flasher_erase_flash(size_t erase_size) {
    uint8_t prev_progress = UINT8_MAX;

    size_t size_erased;
    for(size_erased = 0; size_erased < erase_size;) {
        if(!flasher_erase_sector(size_erased)) {
            FURI_LOG_E(TAG, "Failed to erase flash sector at address 0x%zX", size_erased);
            flasher_emit_error(FlasherErrorDisconnect);
            break;
        }

        size_erased += MIN(erase_size - size_erased, W25Q128_SECTOR_SIZE);

        const uint8_t erase_progress = (size_erased * 100UL) / erase_size;
        if(erase_progress != prev_progress) {
            prev_progress = erase_progress;
            flasher_emit_progress(PROGRESS_VERIFY_WEIGHT, PROGRESS_ERASE_WEIGHT, erase_progress);
            FURI_LOG_D(TAG, "Erasing flash: %u%%", erase_progress);
        }
    }

    return size_erased == erase_size;
}

static bool flasher_program_flash(File* file, size_t data_size) {
    uint8_t prev_progress = UINT8_MAX;

    size_t size_programmed;
    for(size_programmed = 0; size_programmed < data_size;) {
        uint8_t buf[W25Q128_PAGE_SIZE];

        if(!uf2_read_block(file, buf, W25Q128_PAGE_SIZE)) {
            FURI_LOG_E(TAG, "Failed to read UF2 block");
            flasher_emit_error(FlasherErrorBadFile);
            break;
        }

        if(!flasher_program_page(size_programmed, buf, W25Q128_PAGE_SIZE)) {
            FURI_LOG_E(TAG, "Failed to program flash page at address 0x%zX", size_programmed);
            flasher_emit_error(FlasherErrorDisconnect);
            break;
        }

        size_programmed += W25Q128_PAGE_SIZE;

        const uint8_t program_progress = (size_programmed * 100UL) / data_size;
        if(program_progress != prev_progress) {
            prev_progress = program_progress;
            flasher_emit_progress(
                PROGRESS_VERIFY_WEIGHT + PROGRESS_ERASE_WEIGHT,
                PROGRESS_PROGRAM_WEIGHT,
                program_progress);
            FURI_LOG_D(TAG, "Programming flash: %u%%", program_progress);
        }
    }

    return size_programmed == data_size;
}

void flasher_start(const char* file_path) {
    FURI_LOG_D(TAG, "Flashing firmware from file: %s", file_path);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(storage);
    size_t data_size;

    do {
        if(!flasher_prepare_target()) break;
        if(!flasher_prepare_file(file, file_path)) break;
        if(!flasher_verify_file(file, &data_size)) break;
        if(!flasher_erase_flash(data_size)) break;
        if(!flasher_program_flash(file, data_size)) break;
        flasher_emit_success();
    } while(false);

    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}
