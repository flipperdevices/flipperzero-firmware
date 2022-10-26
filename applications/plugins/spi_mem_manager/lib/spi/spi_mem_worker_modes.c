#include "spi_mem_worker.h"
// #include "../../spi_mem_files.h"

static void spi_mem_chip_detect_process(SPIMemWorker* worker);
static void spi_mem_read_process(SPIMemWorker* worker);

const SPIMemWorkerModeType spi_mem_worker_modes[] = {
    [SPIMemWorkerModeIdle] = {.process = NULL},
    [SPIMemWorkerModeChipDetect] = {.process = spi_mem_chip_detect_process},
    [SPIMemWorkerModeRead] = {.process = spi_mem_read_process}};

// ChipDetect
static void spi_mem_chip_detect_process(SPIMemWorker* worker) {
    SPIMemCustomEventWorker event;
    while(!spi_mem_tools_read_chip_info(worker->chip_info)) {
        if(spi_mem_worker_check_for_stop(worker)) return;
    }
    if(spi_mem_chip_complete_info(worker->chip_info)) {
        event = SPIMemCustomEventWorkerChipIdentified;
    } else {
        event = SPIMemCustomEventWorkerChipUnknown;
    }
    if(worker->callback) {
        worker->callback(worker->cb_ctx, event);
    }
}

// Read
static void spi_mem_read_process(SPIMemWorker* worker) {
    FlipperFormat* file = flipper_format_file_alloc(worker->storage);
    uint8_t data_buffer[SPI_MEM_MAX_BLOCK_SIZE];
    size_t offset = 0;
    while(true) {
        if(spi_mem_worker_check_for_stop(worker)) break;
        if((offset + SPI_MEM_MAX_BLOCK_SIZE) >= worker->chip_info->size) break;
        spi_mem_tools_read_block_data(
            worker->chip_info, SPI_MEM_MAX_BLOCK_SIZE, offset, data_buffer);
        // spi_mem_files_write_block_data(worker->cb_ctx, FlipperFormat* file, SPI_MEM_MAX_BLOCK_SIZE, data_buffer)
        offset += SPI_MEM_MAX_BLOCK_SIZE;
        if(worker->callback) {
            worker->callback(worker->cb_ctx, SPIMemCustomEventWorkerBlockReaded);
        }
    }
    flipper_format_file_close(file);
    flipper_format_free(file);
    if(worker->callback) {
        worker->callback(worker->cb_ctx, SPIMemCustomEventWorkerReadDone);
    }
}
