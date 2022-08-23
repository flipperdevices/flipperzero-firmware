#include <furi_hal.h>
#include <furi_hal_memory.h>

#define TAG "FuriHalMemory"

typedef enum {
    SRAM_A,
    SRAM_B,
    SRAM_MAX,
} SRAM;

typedef struct {
    uint8_t* start;
    uint32_t size;
} FuriHalMemoryRegion;

typedef struct {
    FuriHalMemoryRegion region[2];
    bool enabled;
} FuriHalMemory;

static FuriHalMemory* furi_hal_memory;

extern const void __sram2a_start__;
extern const void __sram2a_free__;
extern const void __sram2b_start__;

void furi_hal_memory_init() {
    FuriHalMemory* memory = malloc(sizeof(FuriHalMemory));
    furi_hal_memory = memory;
    memory->enabled = false;

    if(!ble_glue_wait_for_c2_start(FURI_HAL_BT_C2_START_TIMEOUT)) {
        FURI_LOG_E(TAG, "C2 start timeout");
        return;
    }

    const BleGlueC2Info* c2_ver = ble_glue_get_c2_info();
    uint32_t sram2a_busy_size = (uint32_t)&__sram2a_free__ - (uint32_t)&__sram2a_start__;
    uint32_t sram2a_unprotected_size = 0;
    uint32_t sram2b_unprotected_size = 0;

    if(c2_ver->mode == BleGlueC2ModeStack) {
        sram2a_unprotected_size = (32 - c2_ver->MemorySizeSram2A) * 1024;
        sram2b_unprotected_size = (32 - c2_ver->MemorySizeSram2B) * 1024;
    } else if(c2_ver->mode == BleGlueC2ModeFUS) {
        sram2a_unprotected_size = (32 - c2_ver->FusMemorySizeSram2A) * 1024;
        sram2b_unprotected_size = (32 - c2_ver->FusMemorySizeSram2B) * 1024;
    }

    memory->region[SRAM_A].start = (uint8_t*)&__sram2a_free__;
    memory->region[SRAM_B].start = (uint8_t*)&__sram2b_start__;

    if(sram2a_unprotected_size > sram2a_busy_size) {
        memory->region[SRAM_A].size = sram2a_unprotected_size - sram2a_busy_size;
    } else {
        memory->region[SRAM_A].size = 0;
    }
    memory->region[SRAM_B].size = sram2b_unprotected_size;

    FURI_LOG_I(TAG, "SRAM2A: 0x%p, %d", memory->region[SRAM_A].start, memory->region[SRAM_A].size);
    FURI_LOG_I(TAG, "SRAM2B: 0x%p, %d", memory->region[SRAM_B].start, memory->region[SRAM_B].size);

    if((memory->region[SRAM_A].size > 0) || (memory->region[SRAM_B].size > 0)) {
        if((memory->region[SRAM_A].size > 0)) {
            FURI_LOG_I(TAG, "SRAM2A clear");
            memset(memory->region[SRAM_A].start, 0, memory->region[SRAM_A].size);
        }
        if((memory->region[SRAM_B].size > 0)) {
            LL_SYSCFG_DisableSRAMFetch();
            FURI_LOG_I(TAG, "SRAM2B clear");
            memset(memory->region[SRAM_B].start, 0, memory->region[SRAM_B].size);
        }
        memory->enabled = true;
        FURI_LOG_I(TAG, "Enabled");
    } else {
        FURI_LOG_E(TAG, "No SRAM2 available");
    }
}