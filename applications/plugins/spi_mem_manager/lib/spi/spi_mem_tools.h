#pragma once

#include "spi_mem_chip.h"

#define SPI_MEM_SPI_TIMEOUT 1000
#define SPI_MEM_MAX_BLOCK_SIZE 256

bool spi_mem_tools_read_chip_info(SPIMemChip* chip);
bool spi_mem_tools_read_block_data(SPIMemChip* chip, size_t size, size_t offset, uint8_t* data);
