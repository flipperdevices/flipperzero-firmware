#pragma once

#include <furi.h>
#include <flipper_format/flipper_format.h>
#include "spi_mem_chip.h"
#include "spi_mem_tools.h"

typedef enum {
    SPIMemWorkerModeIdle,
    SPIMemWorkerModeChipDetect,
    SPIMemWorkerModeRead
} SPIMemWorkerMode;

typedef enum {
    SPIMemCustomEventWorkerChipIdentified,
    SPIMemCustomEventWorkerChipUnknown
} SPIMemCustomEventWorker;

typedef enum { SPIMemWorkerChipIdentified, SPIMemWorkerChipUnknown } SPIMemWorkerResult;

typedef void (*SPIMemWorkerChipDetectCallback)(void* context, SPIMemWorkerResult result);
typedef void (*SPIMemWorkerReadCallback)(void* context, SPIMemWorkerResult result);

typedef struct {
    SPIMemChip* chip_info;
    SPIMemWorkerMode mode_index;
    SPIMemWorkerChipDetectCallback chip_detect_cb;
    SPIMemWorkerReadCallback read_cb;
    void* cb_ctx;
    FuriThread* thread;
    FuriString* file_name;
} SPIMemWorker;

typedef struct {
    void (*const process)(SPIMemWorker* worker);
} SPIMemWorkerModeType;

extern const SPIMemWorkerModeType spi_mem_worker_modes[];

SPIMemWorker* spi_mem_worker_alloc();

void spi_mem_worker_free(SPIMemWorker* worker);
void spi_mem_worker_start_thread(SPIMemWorker* worker);
void spi_mem_worker_stop_thread(SPIMemWorker* worker);
bool spi_mem_worker_check_for_stop(SPIMemWorker* worker);
void spi_mem_worker_chip_detect_start(
    SPIMemChip* chip_info,
    SPIMemWorker* worker,
    SPIMemWorkerChipDetectCallback callback,
    void* context);
void spi_mem_worker_read_start(
    SPIMemChip* chip_info,
    SPIMemWorker* worker,
    SPIMemWorkerReadCallback callback,
    void* context);
