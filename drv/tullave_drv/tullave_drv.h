#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct TuLlaveInfo {
    uint32_t balance;
    uint8_t* card_number;
};

typedef struct TuLlaveInfo TuLlaveInfo;

void tullave_drv_exit_sleep();
void tullave_drv_start_sleep();
bool tullave_drv_req_card_info(TuLlaveInfo* card_info);