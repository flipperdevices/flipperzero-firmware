#include "platform.h"
#include <assert.h>
#include <main.h>

HAL_StatusTypeDef platformSpiTxRx(const uint8_t *txBuf, uint8_t *rxBuf, uint16_t len) {
    HAL_StatusTypeDef ret;
    if (txBuf && rxBuf) {
        ret = HAL_SPI_TransmitReceive(&SPI_R, (uint8_t*)txBuf, rxBuf, len, HAL_MAX_DELAY);
    } else if (txBuf) {
        ret = HAL_SPI_Transmit(&SPI_R, (uint8_t*)txBuf, len, HAL_MAX_DELAY);
    } else if (rxBuf) {
        ret = HAL_SPI_Receive(&SPI_R, (uint8_t*)rxBuf, len, HAL_MAX_DELAY);
    }
    
    if(ret != HAL_OK) {
        exit(250);
    }
    return ret;
}


void platformProtectST25RComm()
{
    if (HAL_SPI_DeInit(&SPI_R) != HAL_OK) {
        Error_Handler();
    }

    SPI_R.Init.Mode = SPI_MODE_MASTER;
    SPI_R.Init.Direction = SPI_DIRECTION_2LINES;
    SPI_R.Init.DataSize = SPI_DATASIZE_8BIT;
    SPI_R.Init.CLKPolarity = SPI_POLARITY_LOW;
    SPI_R.Init.CLKPhase = SPI_PHASE_2EDGE;
    SPI_R.Init.NSS = SPI_NSS_SOFT;
    SPI_R.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
    SPI_R.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI_R.Init.TIMode = SPI_TIMODE_DISABLE;
    SPI_R.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SPI_R.Init.CRCPolynomial = 7;
    SPI_R.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    SPI_R.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

    if (HAL_SPI_Init(&SPI_R) != HAL_OK) {
        Error_Handler();
    }
}

void platformUnprotectST25RComm()
{
    
}
