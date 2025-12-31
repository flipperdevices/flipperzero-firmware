/**
 * @file nfc_logger.h
 * @brief NFC logger
 *
 * This is a helper module inside of NFC library, which can be very
 * helpful during inspecting NFC tags and protocols. It allows to take
 * a look on data transmissopn payloads between device and tag in
 * both Listener and Poller modes.
 *
 * By default this feature is disabled and does minimal impact on
 * NFC subsystem.
 */
#pragma once

#include <furi.h>
#include <furi_hal_nfc.h>
#include <stream/file_stream.h>

#include <nfc/protocols/nfc_protocol.h>
#include "transaction/nfc_transaction.h"
#include "history/nfc_history.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcLogger NfcLogger;

/**
 * @brief Allows to config Logger from the application. This function must be called
 * before starting poller or listener.
 *
 * @param[in] instance pointer to an NfcLogger instance.
 * @param[in] enabled setting this to true enables logger.
 * @param[in] log_folder_path path to the folder where log files will be created.
 * Folder will be created once logger will be enabled for the first time.
 */
void nfc_logger_config(NfcLogger* instance, bool enabled, const char* log_folder_path);

/**
 * @brief Checks whether new binary log file is present in log folder.
 *
 * @param[in] instance pointer to an NfcLogger instance.
 * @param[out] output path to binary file which has not been processed yet.
 *
 * @return true when there is an unprocessed file, otherwise false.
 */
bool nfc_logger_raw_log_file_present(NfcLogger* instance, FuriString* output);
#ifdef __cplusplus
}
#endif
