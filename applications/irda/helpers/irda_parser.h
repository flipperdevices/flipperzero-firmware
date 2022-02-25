/**
  * @file irda_parser.h
  * Infrared: Helper file for conversion Flipper File Format
  *     to Infrared signal class, and backwards
  */
#pragma once

#include "../irda_app_signal.h"
#include <flipper_format/flipper_format.h>
#include <string>

/** Save Irda signal into file
 *
 * @param ff - Flipper File Format instance
 * @param signal - Irda signal to save
 * @param name - name for saved signal. Every
 *      signal on disk has name.
 */
bool irda_parser_save_signal(
    FlipperFormat* ff,
    const IrdaAppSignal& signal,
    const std::string& name);

/** Read Irda signal from file
 *
 * @param ff - Flipper File Format instance
 * @param signal - Irda signal to read to
 * @param name - name for saved signal. Every
 *      signal in file has name.
 */
bool irda_parser_read_signal(FlipperFormat* ff, IrdaAppSignal& signal, std::string& name);

/** Validate parsed signal
 *
 * @signal - signal to validate
 * @retval true if valid, false otherwise
 */
bool irda_parser_is_parsed_signal_valid(const IrdaMessage* signal);

/** Validate raw signal
 *
 * @signal - signal to validate
 * @retval true if valid, false otherwise
 */
bool irda_parser_is_raw_signal_valid(uint32_t frequency, float duty_cycle, uint32_t timings_cnt);
