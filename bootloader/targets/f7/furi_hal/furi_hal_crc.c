#include <furi_hal.h>
#include <stm32wbxx_hal_crc.h>
#include <stm32wbxx_hal_rcc.h>

static CRC_HandleTypeDef hcrc = {
    .Instance = CRC,
    .Init.CRCLength = CRC_POLYLENGTH_32B,
    .Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE,
    .Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE,
    .Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE,
    .Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE,
    .InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES,
};

void furi_hal_crc_init() {
    __HAL_RCC_CRC_CLK_ENABLE();
    furi_assert(HAL_CRC_Init(&hcrc) == HAL_OK);
}

void furi_hal_crc_deinit() {
    __HAL_RCC_CRC_CLK_DISABLE();
}

void furi_hal_crc_reset() {
    /* Reset CRC Calculation Unit (hcrc->Instance->INIT is
     *  written in hcrc->Instance->DR) */
    __HAL_CRC_DR_RESET(&hcrc);
}

uint32_t furi_hal_crc_feed(void* data, uint16_t length) {
    return ~HAL_CRC_Accumulate(&hcrc, data, length);
}