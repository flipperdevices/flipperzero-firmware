#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void tullave_drv_send_apdu_command(
    uint8_t* req_buffer,
    size_t req_size,
    uint8_t* resp_buffer,
    size_t resp_size);

bool tullave_drv_detect_tullave_card();