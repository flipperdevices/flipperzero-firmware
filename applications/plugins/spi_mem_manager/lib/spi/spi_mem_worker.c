#include "spi_mem_worker.h"

typedef enum {
    SPIMemEventStopThread = (1 << 0),
    SPIMemEventChipDetect = (1 << 1),
    SPIMemEventRead = (1 << 2),
    SPIMemEventAll = (SPIMemEventStopThread | SPIMemEventChipDetect | SPIMemEventRead)
} SPIMemEventEventType;

static int32_t spi_mem_worker_thread(void* thread_context);

SPIMemWorker* spi_mem_worker_alloc() {
    SPIMemWorker* worker = malloc(sizeof(SPIMemWorker));
    worker->callback = NULL;
    worker->thread = furi_thread_alloc();
    worker->mode_index = SPIMemWorkerModeIdle;
    furi_thread_set_name(worker->thread, "SPIMemWorker");
    furi_thread_set_callback(worker->thread, spi_mem_worker_thread);
    furi_thread_set_context(worker->thread, worker);
    furi_thread_set_stack_size(worker->thread, 1024);
    return worker;
}

void spi_mem_worker_free(SPIMemWorker* worker) {
    furi_thread_free(worker->thread);
    free(worker);
}

bool spi_mem_worker_check_for_stop(SPIMemWorker* worker) {
    UNUSED(worker);
    uint32_t flags = furi_thread_flags_get();
    return (flags & SPIMemEventStopThread);
}

static int32_t spi_mem_worker_thread(void* thread_context) {
    SPIMemWorker* worker = thread_context;
    while(true) {
        uint32_t flags = furi_thread_flags_wait(SPIMemEventAll, FuriFlagWaitAny, FuriWaitForever);
        if(flags != FuriFlagErrorTimeout) {
            if(flags & SPIMemEventStopThread) break;
            if(flags & SPIMemEventChipDetect) worker->mode_index = SPIMemWorkerModeChipDetect;
            if(flags & SPIMemEventRead) worker->mode_index = SPIMemWorkerModeRead;
            if(spi_mem_worker_modes[worker->mode_index].process) {
                spi_mem_worker_modes[worker->mode_index].process(worker);
            }
            worker->mode_index = SPIMemWorkerModeIdle;
        }
    }
    return 0;
}

void spi_mem_worker_start_thread(SPIMemWorker* worker) {
    furi_thread_start(worker->thread);
}

void spi_mem_worker_stop_thread(SPIMemWorker* worker) {
    furi_thread_flags_set(furi_thread_get_id(worker->thread), SPIMemEventStopThread);
    furi_thread_join(worker->thread);
}

void spi_mem_worker_chip_detect_start(
    SPIMemChip* chip_info,
    SPIMemWorker* worker,
    SPIMemWorkerCallback callback,
    Storage* storage,
    void* context) {
    furi_assert(worker->mode_index == SPIMemWorkerModeIdle);
    worker->callback = callback;
    worker->cb_ctx = context;
    worker->chip_info = chip_info;
    worker->storage = storage;
    furi_thread_flags_set(furi_thread_get_id(worker->thread), SPIMemEventChipDetect);
}

void spi_mem_worker_read_start(
    SPIMemChip* chip_info,
    SPIMemWorker* worker,
    SPIMemWorkerCallback callback,
    Storage* storage,
    void* context) {
    furi_assert(worker->mode_index == SPIMemWorkerModeIdle);
    worker->callback = callback;
    worker->cb_ctx = context;
    worker->chip_info = chip_info;
    worker->storage = storage;
    furi_thread_flags_set(furi_thread_get_id(worker->thread), SPIMemEventRead);
}
