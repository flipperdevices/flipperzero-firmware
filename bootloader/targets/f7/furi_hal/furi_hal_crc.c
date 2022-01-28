#include <furi_hal_crc.h>
#include <stm32wbxx_hal_crc.h>
#include <stm32wbxx_hal_rcc.h>

void furi_hal_crc_init() {
    __HAL_RCC_CRC_CLK_ENABLE();
}

void furi_hal_crc_deinit() {
    __HAL_RCC_CRC_CLK_DISABLE();
}

void furi_hal_crc_reset() {
}

void furi_hal_crc_feed(void* data, uint16_t length) {
}

uint32_t furi_hal_crc_get() {
    return 0;
}