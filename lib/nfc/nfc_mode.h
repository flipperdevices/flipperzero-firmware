#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of possible operating modes.
 *
 * Not all technologies implement the listener operating mode.
 */
typedef enum {
    NfcModePoller, /**< Configure the Nfc instance as a poller. */
    NfcModeListener, /**< Configure the Nfc instance as a listener. */

    NfcModeNum, /**< Operating mode count. Internal use. */
} NfcMode;

#ifdef __cplusplus
}
#endif