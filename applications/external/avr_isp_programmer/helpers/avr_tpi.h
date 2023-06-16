#pragma once

#include <furi_hal.h>

typedef struct AvrTpi AvrTpi;
typedef void (*AvrTpiCallback)(void* context);

struct AvrTpiSignature {
    uint8_t vendor;
    uint8_t part_family;
    uint8_t part_number;
};

typedef struct AvrTpiSignature AvrTpiSignature;

AvrTpi* avr_tpi_alloc(void);

void avr_tpi_free(AvrTpi* instance);

void avr_tpi_set_tx_callback(AvrTpi* instance, AvrTpiCallback callback, void* context);

bool avr_tpi_start_pmode(AvrTpi* instance);

void avr_tpi_end_pmode(AvrTpi* instance);

void avr_tpi_erase_chip(AvrTpi* instance);

AvrTpiSignature avr_tpi_get_signature(AvrTpi* instance);
uint8_t avr_tpi_get_nwm_lock_bit(AvrTpi* instance);
void avr_tpi_set_nwm_lock_bit(AvrTpi* instance, uint8_t value);
uint8_t avr_tpi_get_configuration_bit(AvrTpi* instance);
void avr_tpi_set_configuration_bit(AvrTpi* instance, uint8_t value);
uint8_t avr_tpi_get_calibration_bit(AvrTpi* instance);
void avr_tpi_read_block(AvrTpi* instance, uint16_t address, uint8_t* data, uint16_t size);
void avr_tpi_write_block(AvrTpi* instance, uint16_t address, uint8_t* data, uint16_t size);


