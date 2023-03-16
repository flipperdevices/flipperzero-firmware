#pragma once

#include <furi_hal.h>

typedef struct AvrIsp AvrIsp;
typedef void (*AvrIspCallback)(void* context);

AvrIsp* avr_isp_alloc(void);
void avr_isp_free(AvrIsp* instance);
void avr_isp_set_tx_callback(AvrIsp* instance, AvrIspCallback callback, void* context);
bool avr_isp_auto_set_spi_speed_start_pmode(AvrIsp* instance);
void avr_isp_read_signature(AvrIsp* instance, uint8_t* data);
void avr_isp_end_pmode(AvrIsp* instance);
uint8_t avr_isp_spi_transaction(
    AvrIsp* instance,
    uint8_t cmd,
    uint8_t addr_hi,
    uint8_t addr_lo,
    uint8_t data);
bool avr_isp_read_page(
    AvrIsp* instance,
    uint32_t memtype,
    uint16_t addr,
    uint16_t size_page,
    uint8_t* data,
    uint32_t size_data);