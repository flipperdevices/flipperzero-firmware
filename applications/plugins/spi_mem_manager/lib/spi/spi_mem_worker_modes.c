#include "spi_mem_worker.h"

const SPIMemWorkerModeType spi_mem_worker_modes[] = {
    [SPIMemWorkerIdle] = {.process = NULL},
    [SPIMemWorkerChipDetect] = {.process = NULL}
};

