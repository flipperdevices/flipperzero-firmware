#include "spi_mem_worker_i.h"
#include "spi_mem_chip.h"
#include "spi_mem_tools.h"
#include "../../spi_mem_files.h"

static void spi_mem_chip_detect_process(SPIMemWorker* worker);
static void spi_mem_read_process(SPIMemWorker* worker);

const SPIMemWorkerModeType spi_mem_worker_modes[] = {
    [SPIMemWorkerModeIdle] = {.process = NULL},
    [SPIMemWorkerModeChipDetect] = {.process = spi_mem_chip_detect_process},
    [SPIMemWorkerModeRead] = {.process = spi_mem_read_process}};

void spi_mem_run_worker_callback(SPIMemWorker* worker, SPIMemCustomEventWorker event) {
    if(worker->callback) {
        worker->callback(worker->cb_ctx, event);
    }
}

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
    spi_mem_run_worker_callback(worker, event);
}

// Read
// File already opend by scenes/spi_mem_scene_read_filename.c
static void spi_mem_read_process(SPIMemWorker* worker) {
    uint8_t data_buffer[SPI_MEM_FILE_BUFFER_SIZE];
    size_t chip_size = spi_mem_chip_get_size(worker->chip_info);
    size_t offset = 0;
    bool success = true;
    while(true) {
        if(spi_mem_worker_check_for_stop(worker)) break;
        if((offset + SPI_MEM_FILE_BUFFER_SIZE) >= chip_size) break;
        if(!spi_mem_tools_read_block_data(
               worker->chip_info, offset, data_buffer, SPI_MEM_FILE_BUFFER_SIZE)) {
            spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerReadFail);
            success = false;
            break;
        }
        if(!spi_mem_file_write_block(worker->cb_ctx, data_buffer, SPI_MEM_FILE_BUFFER_SIZE)) {
            spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerWriteFileFailed);
            success = false;
            break;
        }
        offset += SPI_MEM_FILE_BUFFER_SIZE;
        spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerBlockReaded);
    }
    spi_mem_file_close(worker->cb_ctx);
    if(success) spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerReadDone);
}
