#pragma once

#include <furi.h>

typedef struct SPIMemWorker SPIMemWorker;

typedef struct {
    void (*const process)(SPIMemWorker* worker);
} SPIMemWorkerModeType;

typedef enum {
    SPIMemWorkerWriteOK,
} SPIMemWorkerWriteResult;

typedef void (*SPIMemWorkerChipDetectCallback)(void* context);
typedef void (*SPIMemWorkerReadCallback)(void* context);
typedef void (*SPIMemWorkerWriteCallback)(void* context, SPIMemWorkerWriteResult result);

extern const SPIMemWorkerModeType spi_mem_worker_modes[];

typedef enum {
    SPIMemWorkerIdle,
    SPIMemWorkerChipDetect,
    SPIMemWorkerRead,
    SPIMemWorkerWrite
} SPIMemWorkerMode;

SPIMemWorker* spi_mem_worker_alloc();

void spi_mem_worker_free(SPIMemWorker* worker);
void spi_mem_worker_start_thread(SPIMemWorker* worker);
void spi_mem_worker_stop(SPIMemWorker* worker);
void spi_mem_worker_stop_thread(SPIMemWorker* worker);
void spi_mem_worker_chip_detect_start(SPIMemWorker* worker, SPIMemWorkerChipDetectCallback* callback, void* context);
