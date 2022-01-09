#pragma once

#include "../irda_app_signal.h"
#include "irda.h"
#include "irda_worker.h"
#include "m-string.h"
#include <flipper_file.h>
#include <memory>
#include <string>
#include <furi_hal_irda.h>

#define TAG "IrdaParser"

bool irda_parser_save_signal(FlipperFile* ff, const IrdaAppSignal& signal, const std::string& name);
bool irda_parser_read_signal(FlipperFile* ff, IrdaAppSignal& signal, std::string& name);
bool irda_parser_is_parsed_signal_valid(const IrdaMessage* signal);
bool irda_parser_is_raw_signal_valid(uint32_t frequency, float duty_cycle, uint32_t timings_cnt);

