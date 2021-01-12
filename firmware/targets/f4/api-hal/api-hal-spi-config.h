#pragma once
#include "main.h"

const SPI_InitTypeDef sd_fast_spi = {
    .Mode = SPI_MODE_MASTER,
    .Direction = SPI_DIRECTION_2LINES,
    .DataSize = SPI_DATASIZE_8BIT,
    .CLKPolarity = SPI_POLARITY_LOW,
    .CLKPhase = SPI_PHASE_1EDGE,
    .NSS = SPI_NSS_SOFT,
    .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2,
    .FirstBit = SPI_FIRSTBIT_MSB,
    .TIMode = SPI_TIMODE_DISABLE,
    .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
    .CRCPolynomial = 7,
    .CRCLength = SPI_CRC_LENGTH_DATASIZE,
    .NSSPMode = SPI_NSS_PULSE_ENABLE,
};

const SPI_InitTypeDef sd_slow_spi = {
    .Mode = SPI_MODE_MASTER,
    .Direction = SPI_DIRECTION_2LINES,
    .DataSize = SPI_DATASIZE_8BIT,
    .CLKPolarity = SPI_POLARITY_LOW,
    .CLKPhase = SPI_PHASE_1EDGE,
    .NSS = SPI_NSS_SOFT,
    .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256,
    .FirstBit = SPI_FIRSTBIT_MSB,
    .TIMode = SPI_TIMODE_DISABLE,
    .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
    .CRCPolynomial = 7,
    .CRCLength = SPI_CRC_LENGTH_DATASIZE,
    .NSSPMode = SPI_NSS_PULSE_ENABLE,
};

const SPI_InitTypeDef sd_display_spi = {
    .Mode = SPI_MODE_MASTER,
    .Direction = SPI_DIRECTION_2LINES,
    .DataSize = SPI_DATASIZE_8BIT,
    .CLKPolarity = SPI_POLARITY_LOW,
    .CLKPhase = SPI_PHASE_1EDGE,
    .NSS = SPI_NSS_SOFT,
    .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
    .FirstBit = SPI_FIRSTBIT_MSB,
    .TIMode = SPI_TIMODE_DISABLE,
    .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
    .CRCPolynomial = 7,
    .CRCLength = SPI_CRC_LENGTH_DATASIZE,
    .NSSPMode = SPI_NSS_PULSE_ENABLE,
};