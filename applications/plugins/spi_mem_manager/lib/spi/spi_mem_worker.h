#pragma once

#include <furi.h>
#include "spi_mem_chip.h"

typedef enum { SPIMemWorkerModeIdle, SPIMemWorkerModeChipDetect } SPIMemWorkerMode;

typedef enum {
    SPIMemCustomEventWorkerChipDetectSuccess,
    SPIMemCustomEventWorkerChipDetectFail
} SPIMemCustomEventWorker;

typedef enum {
    SPIMemWorkerChipDetectResultSuccess,
    SPIMemWorkerChipDetectResultFail
} SPIMemWorkerChipDetectResult;

typedef void (*SPIMemWorkerChipDetectCallback)(void* context, SPIMemWorkerChipDetectResult result);

typedef struct {
    SPIMemChip* chip_info;
    SPIMemWorkerMode mode_index;
    SPIMemWorkerChipDetectCallback chip_detect_cb;
    void* cb_ctx;
    FuriThread* thread;
} SPIMemWorker;

typedef struct {
    void (*const process)(SPIMemWorker* worker);
} SPIMemWorkerModeType;

extern const SPIMemWorkerModeType spi_mem_worker_modes[];

SPIMemWorker* spi_mem_worker_alloc();

void spi_mem_worker_free(SPIMemWorker* worker);
void spi_mem_worker_start_thread(SPIMemWorker* worker);
void spi_mem_worker_stop(SPIMemWorker* worker);
void spi_mem_worker_stop_thread(SPIMemWorker* worker);
void spi_mem_worker_chip_detect_start(
    SPIMemChip* chip_info,
    SPIMemWorker* worker,
    SPIMemWorkerChipDetectCallback callback,
    void* context);
