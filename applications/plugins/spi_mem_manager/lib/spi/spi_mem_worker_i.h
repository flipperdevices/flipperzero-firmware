#pragma once

#include "spi_mem_worker.h"

typedef enum {
    SPIMemWorkerModeIdle,
    SPIMemWorkerModeChipDetect,
    SPIMemWorkerModeRead,
    SPIMemWorkerModeVerify,
    SPIMemWorkerModeChipErase,
} SPIMemWorkerMode;

struct SPIMemWorker {
    SPIMemChip* chip_info;
    SPIMemWorkerMode mode_index;
    SPIMemWorkerCallback callback;
    void* cb_ctx;
    FuriThread* thread;
    FuriString* file_name;
};

extern const SPIMemWorkerModeType spi_mem_worker_modes[];
