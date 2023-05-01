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

AvrTpi* avr_isp_alloc(void);

void avr_isp_free(AvrTpi* instance);

void avr_isp_set_tx_callback(AvrTpi* instance, AvrTpiCallback callback, void* context);

