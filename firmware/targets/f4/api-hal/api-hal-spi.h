#pragma once
#include "main.h"
#include <cmsis_os.h>

typedef enum {
    SPI_SD_CARD,
    SPI_SD_CARD_INIT,
    SPI_DISPLAY,
} SpiDevice;

void api_hal_spi_init();
void api_hal_spi_lock(SPI_HandleTypeDef* spi);
void api_hal_spi_unlock(SPI_HandleTypeDef* spi);
void api_hal_spi_config_device(SpiDevice device);