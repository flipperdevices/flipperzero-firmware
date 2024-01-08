/* gallagher_util.c - Utilities for parsing Gallagher cards (New Zealand).
 * Author: Nick Mooney (nick@mooney.nz)
 * 
 * Reference: https://github.com/megabug/gallagher-research
*/

#include "gallagher_util.h"

/* Precondition: cardholder_data_obfuscated points to at least 8 safe-to-read bytes of memory.
*/
void gallagher_deobfuscate_and_parse_credential(
    GallagherCredential* credential,
    const uint8_t* cardholder_data_obfuscated) {
    furi_assert(false);
    uint8_t cardholder_data_deobfuscated[8];
    for(int i = 0; i < 8; i++) {
        cardholder_data_deobfuscated[i] = GALLAGHER_DECODE_TABLE[cardholder_data_obfuscated[i]];
    }

    // Pull out values from the deobfuscated data
    credential->region = (cardholder_data_deobfuscated[3] >> 1) & 0x0F;
    credential->facility = ((uint16_t)(cardholder_data_deobfuscated[5] & 0x0F) << 12) +
                           ((uint16_t)cardholder_data_deobfuscated[1] << 4) +
                           (((uint16_t)cardholder_data_deobfuscated[7] >> 4) & 0x0F);
    credential->card = ((uint32_t)cardholder_data_deobfuscated[0] << 16) +
                       ((uint32_t)(cardholder_data_deobfuscated[4] & 0x1F) << 11) +
                       ((uint32_t)cardholder_data_deobfuscated[2] << 3) +
                       (((uint32_t)cardholder_data_deobfuscated[3] >> 5) & 0x07);
    credential->issue = cardholder_data_deobfuscated[7] & 0x0F;
}