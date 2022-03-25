#include <furi_hal.h>

typedef enum {
    CRC_State_Reset,
    CRC_State_Ready,
    CRC_State_Busy,
} CRC_State;

typedef struct {
    CRC_State state;
    osMutexId_t mtx;
} HAL_CRC_Control;

static volatile HAL_CRC_Control hal_crc_control = {
    .state = CRC_State_Reset,
    .mtx = NULL,
};

void furi_hal_crc_init(bool synchronize) {
    /* initialize peripheral with default generating polynomial */
    WRITE_REG(CRC->POL, DEFAULT_CRC32_POLY);
    MODIFY_REG(CRC->CR, CRC_CR_POLYSIZE, CRC_POLYLENGTH_32B);

    WRITE_REG(CRC->INIT, DEFAULT_CRC_INITVALUE);

    /* set input data inversion mode */
    MODIFY_REG(CRC->CR, CRC_CR_REV_IN, CRC_INPUTDATA_INVERSION_BYTE);
    /* set output data inversion mode */
    MODIFY_REG(CRC->CR, CRC_CR_REV_OUT, CRC_OUTPUTDATA_INVERSION_ENABLE);

    if(synchronize) {
        hal_crc_control.mtx = osMutexNew(NULL);
    }
    hal_crc_control.state = CRC_State_Ready;
}

void furi_hal_crc_reset() {
    furi_check(hal_crc_control.state == CRC_State_Ready);
    if(hal_crc_control.mtx) {
        osMutexRelease(hal_crc_control.mtx);
    }
    CRC->CR |= CRC_CR_RESET;
}

static uint32_t furi_hal_crc_handle_8(uint8_t pBuffer[], uint32_t BufferLength) {
    uint32_t i; /* input data buffer index */
    uint16_t data;
    __IO uint16_t* pReg;

    /* Processing time optimization: 4 bytes are entered in a row with a single word write,
     * last bytes must be carefully fed to the CRC calculator to ensure a correct type
     * handling by the peripheral */
    for(i = 0U; i < (BufferLength / 4U); i++) {
        CRC->DR = ((uint32_t)pBuffer[4U * i] << 24U) | ((uint32_t)pBuffer[(4U * i) + 1U] << 16U) |
                  ((uint32_t)pBuffer[(4U * i) + 2U] << 8U) | (uint32_t)pBuffer[(4U * i) + 3U];
    }
    /* last bytes specific handling */
    if((BufferLength % 4U) != 0U) {
        if((BufferLength % 4U) == 1U) {
            *(__IO uint8_t*)(__IO void*)(&CRC->DR) = pBuffer[4U * i];
        } else if((BufferLength % 4U) == 2U) {
            data = ((uint16_t)(pBuffer[4U * i]) << 8U) | (uint16_t)pBuffer[(4U * i) + 1U];
            pReg = (__IO uint16_t*)(__IO void*)(&CRC->DR);
            *pReg = data;
        } else if((BufferLength % 4U) == 3U) {
            data = ((uint16_t)(pBuffer[4U * i]) << 8U) | (uint16_t)pBuffer[(4U * i) + 1U];
            pReg = (__IO uint16_t*)(__IO void*)(&CRC->DR);
            *pReg = data;

            *(__IO uint8_t*)(__IO void*)(&CRC->DR) = pBuffer[(4U * i) + 2U];
        }
    }

    /* Return the CRC computed value */
    return CRC->DR;
}

static uint32_t furi_hal_crc_accumulate(uint32_t pBuffer[], uint32_t BufferLength) {
    if(hal_crc_control.mtx) {
        furi_check(osMutexGetOwner(hal_crc_control.mtx) != NULL);
    }
    return furi_hal_crc_handle_8((uint8_t*)pBuffer, BufferLength);
}

uint32_t furi_hal_crc_feed(void* data, uint16_t length) {
    return ~furi_hal_crc_accumulate(data, length);
}

bool furi_hal_crc_acquire(uint32_t timeout) {
    return osMutexAcquire(hal_crc_control.mtx, timeout) == osOK;
}