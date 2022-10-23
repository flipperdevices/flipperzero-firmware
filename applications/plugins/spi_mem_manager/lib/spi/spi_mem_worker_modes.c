#include "spi_mem_worker.h"

static void spi_mem_get_chip_detect_process(SPIMemWorker* worker);

const SPIMemWorkerModeType spi_mem_worker_modes[] = {
    [SPIMemWorkerModeIdle] = {.process = NULL},
    [SPIMemWorkerModeChipDetect] = {.process = spi_mem_get_chip_detect_process}};

// ChipDetect
static void spi_mem_get_chip_detect_process(SPIMemWorker* worker) {
    SPIMemWorkerChipDetectResult result;
    while(!spi_mem_tools_read_chip_info(worker->chip_info)) {
        if(spi_mem_worker_check_for_stop(worker)) return;
    }
    if(spi_mem_chip_complete_info(worker->chip_info)) {
        result = SPIMemWorkerChipDetectResultSuccess;
    } else {
        result = SPIMemWorkerChipDetectResultFail;
    }
    if(worker->chip_detect_cb) {
        worker->chip_detect_cb(worker->cb_ctx, result);
    }
}
