#include "spi_mem_worker.h"

typedef enum {
    SPIMemEventStopThread = (1 << 0),
    SPIMemDEventStopMode = (1 << 1),
    SPIMemEventChipDetect = (1 << 2),
    SPIMemEventAll = (SPIMemEventStopThread | SPIMemDEventStopMode | SPIMemEventChipDetect)
} SPIMemEventEventType;

struct SPIMemWorker {
    SPIMemWorkerMode mode_index;
    SPIMemWorkerChipDetectCallback* chip_detect_cb;
    SPIMemWorkerReadCallback* read_cb;
    SPIMemWorkerWriteCallback* write_cb;
    FuriThread* thread;
    void* cb_ctx;
};

static int32_t spi_mem_worker_thread(void* thread_context);

SPIMemWorker* spi_mem_worker_alloc() {
    SPIMemWorker* worker = malloc(sizeof(SPIMemWorker));
    worker->read_cb = NULL;
    worker->write_cb = NULL;
    worker->thread = furi_thread_alloc();
    worker->mode_index = SPIMemWorkerIdle;
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

static int32_t spi_mem_worker_thread(void* thread_context) {
    SPIMemWorker* worker = thread_context;
    bool running = true;
    while(running) {
        uint32_t flags = furi_thread_flags_wait(SPIMemEventAll, FuriFlagWaitAny, FuriWaitForever);
        if(flags != FuriFlagErrorTimeout) {
            if(flags & SPIMemEventStopThread) break;
            if(flags & SPIMemEventChipDetect) worker->mode_index = SPIMemWorkerChipDetect;
            if(spi_mem_worker_modes[worker->mode_index].process) {
                spi_mem_worker_modes[worker->mode_index].process(worker);
            }
            worker->mode_index = SPIMemWorkerIdle;
        }
    }
    return 0;
}

void spi_mem_worker_start_thread(SPIMemWorker* worker) {
    furi_thread_start(worker->thread);
}

void spi_mem_worker_stop_thread(SPIMemWorker* worker) {
    furi_assert(worker->mode_index == SPIMemWorkerIdle);
    furi_thread_flags_set(furi_thread_get_id(worker->thread), SPIMemEventStopThread);
    furi_thread_join(worker->thread);
}

void spi_mem_worker_stop(SPIMemWorker* worker) {
    furi_thread_flags_set(furi_thread_get_id(worker->thread), SPIMemDEventStopMode);
}

void spi_mem_worker_chip_detect_start(SPIMemWorker* worker, SPIMemWorkerChipDetectCallback* callback, void* context) {
    furi_assert(worker->mode_index == SPIMemWorkerIdle);
    worker->chip_detect_cb = callback;
    worker->cb_ctx = context;
    furi_thread_flags_set(furi_thread_get_id(worker->thread), SPIMemEventChipDetect);
}
