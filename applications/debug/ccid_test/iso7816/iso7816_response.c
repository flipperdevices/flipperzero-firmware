/**
 * @file iso7816_response.c
 * @brief ISO7816-4 Response handling implementation
 */

#include <stdint.h>
#include "iso7816_t0_apdu.h"
#include "iso7816_response.h"

void iso7816_set_response(ISO7816_Response_APDU* responseAPDU, uint16_t responseCode) {
    responseAPDU->SW1 = (responseCode >> 8) & 0xFF;
    responseAPDU->SW2 = responseCode & 0xFF;
}

void iso7816_set_response_len(ISO7816_Response_APDU* responseAPDU, uint8_t sw1, uint8_t len) {
    responseAPDU->SW1 = sw1;
    responseAPDU->SW2 = len;
}
