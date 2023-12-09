/**
 * @file infrared_signal.h
 * @brief This file contains the infrared signal module.
 * @details This file contains the infrared signal module for sending infrared signals from a file.
 * Both raw and parsed signals are supported.  The file can be in either format "IR signals file"
 * or "IR library file".
 * @name There is no 'infrared_signal_alloc' function.  Use 'infrared_signal_load_file'.
*/

#pragma once

#include <furi.h>

#include "resources.h"

typedef struct InfraredSignal InfraredSignal;

/**
 * @brief Load an infrared signal (action) from a file.  
 * @note The first signal is loaded and ready for sending.
 * @param file_path The path to the file to load.
 * @param action The name of the action to load from the file.
 * @param resources The resources to use for sending the signal.
 * @return The loaded signal, or NULL if there was an error.
*/
InfraredSignal* infrared_signal_load_file(char* path, char* action, Resources* resources);

/**
 * @brief Load the next infrared signal (action) from the file.
 * @param signal The signal to load from.
 * @return True if the signal was loaded, false if there are no more signals.
*/
bool infrared_signal_load_next(InfraredSignal* signal);

/**
 * @brief Send the infrared signal (action) to the IR LED.
 * @param signal The signal to send.
 * @return True if the signal was sent, false if there was an error.
*/
bool infrared_signal_send(InfraredSignal* signal);

/**
 * @brief Free the memory used by the signal.
 * @param signal The signal to free.
*/
void infrared_signal_free(InfraredSignal* signal);
