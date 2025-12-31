#pragma once

#include <core/string.h>

#include <nfc/nfc.h>
#include "plugins/logger/nfc_logger_config_data_type.h"

#ifdef __cplusplus
extern "C" {
#endif

void nfc_logger_format(Nfc* nfc, const NfcLoggerFormatterConfig* config);

#ifdef __cplusplus
}
#endif
