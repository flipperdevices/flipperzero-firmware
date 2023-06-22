/**
 * @file fild_validation_rfid_driver.h
 * RFID HAL API
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct FildValdationDriverRfid FildValdationDriverRfid;

FildValdationDriverRfid* fild_validation_rfid_driver_alloc();

void fild_validation_rfid_driver_free(FildValdationDriverRfid* instance);

bool fild_validation_rfid_driver_check(FildValdationDriverRfid* instance, uint32_t* frequency);

void fild_validation_rfid_driver_dma_start(FildValdationDriverRfid* instance);

void fild_validation_rfid_driver_dma_stop(FildValdationDriverRfid* instance);

#ifdef __cplusplus
}
#endif
