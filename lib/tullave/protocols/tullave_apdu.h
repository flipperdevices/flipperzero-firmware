#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../tullave_worker_i.h"

//Debug command: nfc adpu 00a4040007d4100000030001
static const uint8_t auth_req[] =
    {0x00, 0xa4, 0x04, 0x00, 0x07, 0xd4, 0x10, 0x00, 0x00, 0x03, 0x00, 0x01};

//Debug command: nfc adpu 904c000004. Send firs auth_req.
static const uint8_t bal_req[] = {0x90, 0x4c, 0x00, 0x00, 0x04};

void tullave_apdu_nfc_exit_sleep();
void tullave_apdu_nfc_start_sleep();
void tullave_apdu_nfc_stop();
bool tullave_apdu_read(TuLlaveInfo* card_info);
