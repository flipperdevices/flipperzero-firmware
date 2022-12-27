#include "sd_spi_io.h"
#include <furi/core/core_defines.h>

uint8_t sd_max_mount_retry_count() {
    return 10;
}

SDStatus sd_init(bool power_reset) {
    UNUSED(power_reset);
    return SDStatusError;
}

SDStatus sd_get_card_state(void) {
    return SDStatusError;
}

SDStatus sd_get_card_info(SD_CardInfo* card_info) {
    UNUSED(card_info);
    return SDStatusError;
}

SDStatus sd_read_blocks(uint32_t* data, uint32_t address, uint32_t blocks, uint32_t timeout) {
    UNUSED(data);
    UNUSED(address);
    UNUSED(blocks);
    UNUSED(timeout);
    return SDStatusError;
}

SDStatus sd_write_blocks(uint32_t* data, uint32_t address, uint32_t blocks, uint32_t timeout) {
    UNUSED(data);
    UNUSED(address);
    UNUSED(blocks);
    UNUSED(timeout);
    return SDStatusError;
}