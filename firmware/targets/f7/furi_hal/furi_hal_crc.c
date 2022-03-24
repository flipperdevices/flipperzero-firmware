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
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_CRC);
    furi_check(HAL_CRC_Init(&hcrc) == HAL_OK);
}

void furi_hal_crc_deinit() {
    LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_CRC);
}

void furi_hal_crc_reset() {
    /* Reset CRC Calculation Unit (hcrc->Instance->INIT is
     *  written in hcrc->Instance->DR) */
    __HAL_CRC_DR_RESET(&hcrc);
}

static uint32_t
    furi_hal_crc_handle_8(CRC_HandleTypeDef* hcrc, uint8_t pBuffer[], uint32_t BufferLength) {
    uint32_t i; /* input data buffer index */
    uint16_t data;
    __IO uint16_t* pReg;

    /* Processing time optimization: 4 bytes are entered in a row with a single word write,
   * last bytes must be carefully fed to the CRC calculator to ensure a correct type
   * handling by the peripheral */
    for(i = 0U; i < (BufferLength / 4U); i++) {
        hcrc->Instance->DR =
            ((uint32_t)pBuffer[4U * i] << 24U) | ((uint32_t)pBuffer[(4U * i) + 1U] << 16U) |
            ((uint32_t)pBuffer[(4U * i) + 2U] << 8U) | (uint32_t)pBuffer[(4U * i) + 3U];
    }
    /* last bytes specific handling */
    if((BufferLength % 4U) != 0U) {
        if((BufferLength % 4U) == 1U) {
            *(__IO uint8_t*)(__IO void*)(&hcrc->Instance->DR) =
                pBuffer[4U * i]; /* Derogation MisraC2012 R.11.5 */
        }
        if((BufferLength % 4U) == 2U) {
            data = ((uint16_t)(pBuffer[4U * i]) << 8U) | (uint16_t)pBuffer[(4U * i) + 1U];
            pReg = (__IO uint16_t*)(__IO void*)(&hcrc->Instance
                                                     ->DR); /* Derogation MisraC2012 R.11.5 */
            *pReg = data;
        }
        if((BufferLength % 4U) == 3U) {
            data = ((uint16_t)(pBuffer[4U * i]) << 8U) | (uint16_t)pBuffer[(4U * i) + 1U];
            pReg = (__IO uint16_t*)(__IO void*)(&hcrc->Instance
                                                     ->DR); /* Derogation MisraC2012 R.11.5 */
            *pReg = data;

            *(__IO uint8_t*)(__IO void*)(&hcrc->Instance->DR) =
                pBuffer[(4U * i) + 2U]; /* Derogation MisraC2012 R.11.5 */
        }
    }

    /* Return the CRC computed value */
    return hcrc->Instance->DR;
}

static uint32_t
    furi_hal_crc_accumulate(CRC_HandleTypeDef* hcrc, uint32_t pBuffer[], uint32_t BufferLength) {
    uint32_t temp = 0U; /* CRC output (read from hcrc->Instance->DR register) */

    /* Change CRC peripheral state */
    hcrc->State = HAL_CRC_STATE_BUSY;

    furi_assert(hcrc->InputDataFormat == CRC_INPUTDATA_FORMAT_BYTES);

    temp = furi_hal_crc_handle_8(hcrc, (uint8_t*)pBuffer, BufferLength);
    /* Change CRC peripheral state */
    hcrc->State = HAL_CRC_STATE_READY;

    /* Return the CRC computed value */
    return temp;
}

uint32_t furi_hal_crc_feed(void* data, uint16_t length) {
    return ~furi_hal_crc_accumulate(&hcrc, data, length);
}