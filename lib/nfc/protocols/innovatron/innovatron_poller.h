#pragma once

#include "innovatron.h"

#include <nfc/nfc_poller.h>
#include <toolbox/bit_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct InnovatronPoller InnovatronPoller;

typedef enum {
    InnovatronPollerEventTypeError,
    InnovatronPollerEventTypeReady,
} InnovatronPollerEventType;

typedef union {
    InnovatronError error;
} InnovatronPollerEventData;

typedef struct {
    InnovatronPollerEventType type;
    InnovatronPollerEventData* data;
} InnovatronPollerEvent;

/**
 * @brief Perform Innovatron activation procedure.
 *
 * Must ONLY be used inside the callback function.
 *
 * Performs the activation procedure as defined for Innovatron. The data
 * field will be filled with Innovatron data on success.
 *
 * @param[in, out] instance pointer to the instance to be used in the transaction.
 * @param[out] data pointer to the Innovatron data structure to be filled.
 * @return InnovatronErrorNone on success, an error code on failure.
 */
InnovatronError innovatron_poller_activate(InnovatronPoller* instance, InnovatronData* data);

/**
 * @brief Exchange APDUs with the card in poller mode.
 *
 * Must ONLY be used inside the callback function.
 *
 * @param[in, out] instance pointer to the instance to be used in the transaction.
 * @param[in] tx_buffer pointer to the buffer containing C-APDU bytes.
 * @param[out] rx_buffer pointer to the buffer to be filled with R-APDU bytes.
 * @return InnovatronErrorNone on success, an error code on failure.
 */
InnovatronError innovatron_poller_com_r(
    InnovatronPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer);

#ifdef __cplusplus
}
#endif
