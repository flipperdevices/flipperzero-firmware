#include <furi.h>
#include <furi_hal.h>
#include <flipper.h>
#include <alt_boot.h>

#include <fatfs.h>
#include <flipper_format/flipper_format.h>

#include <update_util/update_manifest.h>
#include <update_util/update_operation.h>
#include <toolbox/path.h>
#include <toolbox/crc32_calc.h>

#define UPDATE_POINTER_FILE_PATH "/" UPDATE_MANIFEST_POINTER_FILE_NAME

static FATFS* pfs = NULL;

#define CHECK_FRESULT(result)   \
    {                           \
        if((result) != FR_OK) { \
            return 0;           \
        }                       \
    }

static bool flipper_update_mount_sd(void) {
    for(int i = 0; i < furi_hal_sd_max_mount_retry_count(); ++i) {
        if(furi_hal_sd_init((i % 2) == 0) != FuriStatusOk) {
            /* Next attempt will be without card reset, let it settle */
            furi_delay_ms(1000);
            continue;
        }

        if(f_mount(pfs, "/", 1) == FR_OK) {
            return true;
        }
    }
    return false;
}

static bool flipper_update_init(void) {
    // TODO FL-3504: Configure missing peripherals properly
    furi_hal_bus_enable(FuriHalBusHSEM);
    furi_hal_bus_enable(FuriHalBusIPCC);
    furi_hal_bus_enable(FuriHalBusRNG);
    furi_hal_bus_enable(FuriHalBusUSART1);

    furi_hal_clock_init();
    furi_hal_rtc_init();
    furi_hal_interrupt_init();

    furi_hal_spi_config_init();

    fatfs_init();
    if(!furi_hal_sd_is_present()) {
        return false;
    }

    pfs = malloc(sizeof(FATFS));

    return flipper_update_mount_sd();
}

/**
 * Load and verify boot loader stage from manifest
 * 
 * FatFS structures (FIL, FILINFO) are heap-allocated to reduce stack usage
 * from ~928 bytes to ~100 bytes. This is critical for the boot update path
 * which must complete safely within the main thread's 2048-byte stack.
 * See issue #4332 for stack overflow analysis.
 */
static bool flipper_update_load_stage(const FuriString* work_dir, UpdateManifest* manifest) {
    FIL* file = (FIL*)malloc(sizeof(FIL));      // ~400 bytes - MUST be heap-allocated
    FILINFO* stat = (FILINFO*)malloc(sizeof(FILINFO));  // ~80 bytes

    // Validate allocations to prevent cascade failures
    if(!file || !stat) {
        free(file);
        free(stat);
        return false;
    }

    FuriString* loader_img_path;
    loader_img_path = furi_string_alloc_set(work_dir);
    path_append(loader_img_path, furi_string_get_cstr(manifest->staged_loader_file));

    if((f_stat(furi_string_get_cstr(loader_img_path), stat) != FR_OK) ||
       (f_open(file, furi_string_get_cstr(loader_img_path), FA_OPEN_EXISTING | FA_READ) !=
        FR_OK) ||
       (stat->fsize == 0)) {
        furi_string_free(loader_img_path);
        free(file);
        free(stat);
        return false;
    }
    furi_string_free(loader_img_path);

    void* img = malloc(stat->fsize);
    uint32_t read_total = 0;
    uint16_t read_current = 0;
    const uint16_t MAX_READ = 0xFFFF;

    uint32_t crc = 0;
    do {
        if(f_read(file, img + read_total, MAX_READ, &read_current) != FR_OK) { //-V769
            break;
        }
        crc = crc32_calc_buffer(crc, img + read_total, read_current);
        read_total += read_current;
    } while(read_current == MAX_READ);

    do {
        if((read_total != stat->fsize) || (crc != manifest->staged_loader_crc)) {
            break;
        }

        /* Point of no return. Literally
         *
         * NB: we MUST disable IRQ, otherwise handlers from flash
         * will change global variables (like tick count) 
         * that are located in .data. And we move staged loader 
         * to the same memory region. So, IRQ handlers will mess up 
         * memmove'd .text section and ruin your day. 
         * We don't want that to happen.
         */
        __disable_irq();

        memmove((void*)(SRAM1_BASE), img, stat->fsize);
        LL_SYSCFG_SetRemapMemory(LL_SYSCFG_REMAP_SRAM);
        furi_hal_switch(0x0);
        free(file);
        free(stat);
        return true;

    } while(false);

    free(img);
    free(file);
    free(stat);
    return false;
}

/**
 * Read manifest path from SD card boot pointer file
 * 
 * FatFS structures and path buffer are heap-allocated to reduce stack usage
 * from ~1160 bytes to ~50 bytes. This was the largest single stack consumer
 * in the firmware, exceeding the original 1024-byte limit. Heap allocation
 * is essential for boot path reliability. See issue #4332.
 */
static bool flipper_update_get_manifest_path(FuriString* out_path) {
    FIL* file = (FIL*)malloc(sizeof(FIL));      // ~400 bytes
    FILINFO* stat = (FILINFO*)malloc(sizeof(FILINFO));  // ~80 bytes
    char* manifest_name_buf = (char*)malloc(UPDATE_OPERATION_MAX_MANIFEST_PATH_LEN);  // 256 bytes
    uint16_t size_read = 0;

    // Critical: Validate all allocations to prevent OOM issues during boot
    if(!file || !stat || !manifest_name_buf) {
        free(file);
        free(stat);
        free(manifest_name_buf);
        return false;
    }

    memset(manifest_name_buf, 0, UPDATE_OPERATION_MAX_MANIFEST_PATH_LEN);
    furi_string_reset(out_path);
    CHECK_FRESULT(f_stat(UPDATE_POINTER_FILE_PATH, stat));
    CHECK_FRESULT(f_open(file, UPDATE_POINTER_FILE_PATH, FA_OPEN_EXISTING | FA_READ));
    do {
        if(f_read(file, manifest_name_buf, UPDATE_OPERATION_MAX_MANIFEST_PATH_LEN, &size_read) !=
           FR_OK) {
            break;
        }

        if((size_read == 0) || (size_read == UPDATE_OPERATION_MAX_MANIFEST_PATH_LEN)) {
            break;
        }
        furi_string_set(out_path, manifest_name_buf);
        furi_string_right(out_path, strlen(STORAGE_EXT_PATH_PREFIX));
    } while(0);
    f_close(file);
    free(file);
    free(stat);
    free(manifest_name_buf);
    return !furi_string_empty(out_path);
}

/**
 * Parse and validate firmware update manifest
 * 
 * FatFS structures are heap-allocated to reduce stack usage from ~920 bytes
 * to ~80 bytes. These structures are large and not portable from the stack
 * into deep call chains. Heap allocation provides safety margin for interrupt
 * handlers in the critical boot update path. See issue #4332.
 */
static UpdateManifest* flipper_update_process_manifest(const FuriString* manifest_path) {
    FIL* file = (FIL*)malloc(sizeof(FIL));      // ~400 bytes
    FILINFO* stat = (FILINFO*)malloc(sizeof(FILINFO));  // ~80 bytes

    // Ensure allocations succeed before proceeding
    if(!file || !stat) {
        free(file);
        free(stat);
        return NULL;
    }

    CHECK_FRESULT(f_stat(furi_string_get_cstr(manifest_path), stat));
    CHECK_FRESULT(f_open(file, furi_string_get_cstr(manifest_path), FA_OPEN_EXISTING | FA_READ));

    uint8_t* manifest_data = malloc(stat->fsize);
    uint32_t bytes_read = 0;
    const uint16_t MAX_READ = 0xFFFF;

    do {
        uint16_t size_read = 0;
        if(f_read(file, manifest_data + bytes_read, MAX_READ, &size_read) != FR_OK) { //-V769
            break;
        }
        bytes_read += size_read;
    } while(bytes_read == MAX_READ);

    UpdateManifest* manifest = NULL;
    do {
        if(bytes_read != stat->fsize) {
            break;
        }

        manifest = update_manifest_alloc();
        if(!update_manifest_init_mem(manifest, manifest_data, bytes_read)) {
            update_manifest_free(manifest);
            manifest = NULL;
        }
    } while(false);

    f_close(file);
    free(manifest_data);
    free(file);
    free(stat);
    return manifest;
}

void flipper_boot_update_exec(void) {
    if(!flipper_update_init()) {
        return;
    }

    FuriString* work_dir = furi_string_alloc();
    FuriString* manifest_path = furi_string_alloc();

    do {
        if(!flipper_update_get_manifest_path(manifest_path)) {
            break;
        }

        UpdateManifest* manifest = flipper_update_process_manifest(manifest_path);
        if(!manifest) {
            break;
        }

        path_extract_dirname(furi_string_get_cstr(manifest_path), work_dir);
        if(!flipper_update_load_stage(work_dir, manifest)) {
            update_manifest_free(manifest);
        }
    } while(false);
    furi_string_free(manifest_path);
    furi_string_free(work_dir);
    free(pfs);
}
