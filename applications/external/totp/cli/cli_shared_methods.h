#pragma once

#include <cli/cli.h>
#include "../types/plugin_state.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks whether user is authenticated and entered correct PIN.
 *        If user is not authenticated it prompts user to enter correct PIN to authenticate.
 * @param plugin_state application state
 * @param cli pointer to the firmware CLI subsystem 
 * @return \c true if user is already authenticated or successfully authenticated; \c false otherwise
 */
bool totp_cli_ensure_authenticated(const PluginState* plugin_state, Cli* cli);

/**
 * @brief Forces application to be instantly closed
 * @param event_queue main app queue
 */
void totp_cli_force_close_app(FuriMessageQueue* event_queue);

/**
 * @brief Reads line of characters from console
 * @param cli pointer to the firmware CLI subsystem 
 * @param out_str pointer to an output string to put read line to
 * @param mask_user_input whether to mask input characters in console or not
 * @return \c true if line successfully read and confirmed; \c false otherwise
 */
bool totp_cli_read_line(Cli* cli, FuriString* out_str, bool mask_user_input);

/**
 * @brief Extracts \c uint8_t value and trims arguments string
 * @param args arguments string
 * @param[out] value parsed value
 * @return \c true if value successfully read and parsed as \c uint8_t ; \c false otherwise
 */
bool args_read_uint8_and_trim(FuriString* args, uint8_t* value);

/**
 * @brief Free \c FuriString instance in a secure manner by clearing it first
 * @param str instance to free
 */
void furi_string_secure_free(FuriString* str);

/**
 * @brief Prints information about unknown argument
 * @param arg 
 */
void totp_cli_printf_unknown_argument(const FuriString* arg);

/**
 * @brief Prints information about missed required argument
 * @param arg 
 */
void totp_cli_printf_missed_argument_value(char* arg);

#ifdef __cplusplus
}
#endif
