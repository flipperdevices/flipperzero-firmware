#pragma once

#include <furi.h>
#include "resources.h"

typedef struct SubGhzSignal SubGhzSignal;

/**
 * @brief Loads a SubGhzSignal from a file (supports RAW and Protocol .SUB files)
 * @param file_path The path to the file to load.
 * @return The loaded SubGhzSignal. Be sure to call subghz_signal_free.
*/
SubGhzSignal* subghz_signal_load_file(char* path);

/**
 * @brief Frees a SubGhzSignal.
 * @param signal The SubGhzSignal to free.
*/
void subghz_signal_free(SubGhzSignal* signal);

/**
 * @brief Sends a SubGhzSignal.
 * @param signal The SubGhzSignal to send.
 * @param use_external_radio Currently only the internal radio is supported (please use 'false').  Whether or not to use the external radio.
 * @param resources Resources to use for sending the signal.
*/
void subghz_signal_send(SubGhzSignal* signal, bool use_external_radio, Resources* resources);
