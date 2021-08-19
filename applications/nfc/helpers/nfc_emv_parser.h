#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <m-string.h>

/** Get EMV application name by number
 * @param aid - AID number array
 * @param aid_len - AID length
 * @param aid_name - string to keep AID name
 * @return - true if AID found, false otherwies
 */
bool nfc_emv_parser_get_aid_name(uint8_t* aid, uint8_t aid_len, string_t aid_name);
