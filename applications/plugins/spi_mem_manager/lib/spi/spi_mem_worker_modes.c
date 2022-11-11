#include "spi_mem_worker_i.h"
#include "spi_mem_chip.h"
#include "spi_mem_tools.h"
#include "../../spi_mem_files.h"

static void spi_mem_chip_detect_process(SPIMemWorker* worker);
static void spi_mem_read_process(SPIMemWorker* worker);
static void spi_mem_verify_process(SPIMemWorker* worker);

const SPIMemWorkerModeType spi_mem_worker_modes[] = {
    [SPIMemWorkerModeIdle] = {.process = NULL},
    [SPIMemWorkerModeChipDetect] = {.process = spi_mem_chip_detect_process},
    [SPIMemWorkerModeRead] = {.process = spi_mem_read_process},
    [SPIMemWorkerModeVerify] = {.process = spi_mem_verify_process}};

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
        size_t block_size = SPI_MEM_FILE_BUFFER_SIZE;
        if(spi_mem_worker_check_for_stop(worker)) break;
        if(offset >= chip_size) break;
        if((offset + block_size) > chip_size)
            block_size = chip_size - offset;
        if(!spi_mem_tools_read_block_data(
               worker->chip_info, offset, data_buffer, block_size)) {
            spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerChipReadFail);
            success = false;
            break;
        }
        if(!spi_mem_file_write_block(worker->cb_ctx, data_buffer, block_size)) {
            spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerWriteFileFail);
            success = false;
            break;
        }
        offset += block_size;
        spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerBlockReaded);
    }
    spi_mem_file_close(worker->cb_ctx);
    if(success) spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerReadDone);
}

// Verify
static void spi_mem_verify_process(SPIMemWorker* worker) {
    uint8_t data_buffer_chip[SPI_MEM_FILE_BUFFER_SIZE];
    uint8_t data_buffer_file[SPI_MEM_FILE_BUFFER_SIZE];
    size_t chip_size = spi_mem_chip_get_size(worker->chip_info);
    size_t offset = 0;
    bool success = true;
    if(!spi_mem_file_open(worker->cb_ctx)) return;
    while(true) {
        size_t block_size = SPI_MEM_FILE_BUFFER_SIZE;
        if(spi_mem_worker_check_for_stop(worker)) break;
        if(offset >= chip_size) break;
        if((offset + block_size) > chip_size)
            block_size = chip_size - offset;
        if(!spi_mem_tools_read_block_data(
               worker->chip_info, offset, data_buffer_chip, block_size)) {
            spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerChipReadFail);
            success = false;
            break;
        }
        if(!spi_mem_file_read_block(worker->cb_ctx, data_buffer_file, block_size)) {
            spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerReadFileFail);
            success = false;
            break;
        }
        if(memcmp(data_buffer_chip, data_buffer_file, block_size) != 0) {
            spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerVerifyFail);
            success = false;
            break;
        }
        offset += block_size;
        spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerBlockReaded);
    }
    spi_mem_file_close(worker->cb_ctx);
    if(success) spi_mem_run_worker_callback(worker, SPIMemCustomEventWorkerVerifyDone);
}
