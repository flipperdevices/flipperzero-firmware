#ifndef TAG_SUBGHZ_HEADERS
#define TAG_SUBGHZ_HEADERS

#include <core/check.h>
#include <lib/subghz/subghz_tx_rx_worker.h>

typedef enum {
    SubGhzUninitialised,
    SubGhzReady,
    SubGhzListening,
} TagSubGhzMode;

#define APP_NAME "tag:"
#define MESSAGE_MAX_LENGTH 60
#define PROTOCOL_VERSION '1'

/**
 * Current state of the subghz.
 */
TagSubGhzMode tag_subghz_mode_get();

/**
 * Initialise the subghz module.
 */
void tag_subghz_init();

#endif