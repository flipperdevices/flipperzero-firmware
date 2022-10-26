#include "spi_mem_worker.h"
#include "../../spi_mem_files.h"

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
// File already opend by scenes/spi_mem_scene_read_filename.c
static void spi_mem_read_process(SPIMemWorker* worker) {
    uint8_t data_buffer[SPI_MEM_MAX_BLOCK_SIZE];
    memset(data_buffer, 0xFF, SPI_MEM_MAX_BLOCK_SIZE);
    size_t offset = 0;
    while(true) {
        if(spi_mem_worker_check_for_stop(worker)) break;
        if((offset + SPI_MEM_MAX_BLOCK_SIZE) >= worker->chip_info->size) break;
        // spi_mem_tools_read_block_data(
        //    worker->chip_info, SPI_MEM_MAX_BLOCK_SIZE, offset, data_buffer);
        spi_mem_file_write_block(worker->cb_ctx, data_buffer, SPI_MEM_MAX_BLOCK_SIZE);
        offset += SPI_MEM_MAX_BLOCK_SIZE;
        if(worker->callback) {
            worker->callback(worker->cb_ctx, SPIMemCustomEventWorkerBlockReaded);
        }
    }
    spi_mem_file_close(worker->cb_ctx);
    if(worker->callback) {
        worker->callback(worker->cb_ctx, SPIMemCustomEventWorkerReadDone);
    }
}
