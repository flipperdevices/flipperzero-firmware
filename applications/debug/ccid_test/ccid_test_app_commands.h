/**
 * @file ccid_test_app_commands.h
 * @brief CCID Test Application Commands
 * 
 * Custom command handlers for the test application.
 * Uses CLA=0x01 for test commands.
 */

#pragma once

#include "iso7816/iso7816_t0_apdu.h"
#include "iso7816/iso7816_atr.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Provide Answer-to-Reset
 * 
 * @param atr ATR structure to fill
 */
void iso7816_answer_to_reset(Iso7816Atr* atr);

/**
 * @brief Process application-specific commands
 * 
 * Handles test commands with CLA=0x01.
 * Returns false for standard ISO7816 commands (CLA=0x00) to allow
 * default processing.
 * 
 * @param command_apdu Command APDU
 * @param response_apdu Response APDU to fill
 * @return true if command was handled, false to use default handler
 */
bool iso7816_process_command(
    const ISO7816_Command_APDU* command_apdu,
    ISO7816_Response_APDU* response_apdu);

#ifdef __cplusplus
}
#endif
