#pragma once

#include <furi_hal.h>

typedef enum {
    AvrIspTpiSwSpeed1Mhz = 0,
    AvrIspTpiSwSpeed400Khz = 1,
    AvrIspTpiSwSpeed250Khz = 2,
    AvrIspTpiSwSpeed125Khz = 4,
    AvrIspTpiSwSpeed60Khz = 8,
    AvrIspTpiSwSpeed40Khz = 12,
    AvrIspTpiSwSpeed20Khz = 24,
    AvrIspTpiSwSpeed10Khz = 48,
    AvrIspTpiSwSpeed5Khz = 96,
    AvrIspTpiSwSpeed1Khz = 480,
} AvrIspTpiSwSpeed;

typedef struct AvrIspTpiSw AvrIspTpiSw;

AvrIspTpiSw* avr_isp_tpi_sw_init(AvrIspTpiSwSpeed speed);
void avr_isp_tpi_sw_free(AvrIspTpiSw* instance);
void avr_isp_tpi_sw_tx(AvrIspTpiSw* instance, uint8_t data);
bool avr_isp_tpi_sw_rx(AvrIspTpiSw* instance, uint8_t* data);
//void avr_isp_tpi_sw_res_set(AvrIspTpiSw* instance, bool state);
//void avr_isp_tpi_sw_sck_set(AvrIspTpiSw* instance, bool state);

void avr_isp_tpi_sw_start_pmode(AvrIspTpiSw* instance);
void avr_isp_tpi_sw_end_pmode(AvrIspTpiSw* instance);
