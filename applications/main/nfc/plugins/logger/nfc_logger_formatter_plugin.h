#pragma once

#include <furi/core/string.h>

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>
#include "nfc_logger_config_data_type.h"

/**
 * @brief Unique string identifier for logger plugin.
 */
#define NFC_LOGGER_FORMATTER_PLUGIN_APP_ID "NfcLoggerFormatterPlugin"

/**
 * @brief Currently supported plugin API version.
 */
#define NFC_LOGGER_FORMATTER_PLUGIN_API_VERSION 1

typedef void (*NfcLoggerFormatterPluginFormat)(Nfc* nfc, const NfcLoggerFormatterConfig* config);

/**
 * @brief Plugin interface.
 */
typedef struct {
    NfcLoggerFormatterPluginFormat format;
} NfcLoggerFormatterPlugin;
