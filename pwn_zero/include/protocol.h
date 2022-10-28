#pragma once

#include <furi.h>

typedef struct {
    /// i Location to operate on
    uint8_t i;
    /// j Location to operate on
    uint8_t j;
    /// Code to apply to that pixel location
    uint8_t code;
} PwnCommand;

/// Length of the protocol transmission in bytes
#define PWNAGOTCHI_PROTOCOL_BYTE_LEN 5

/// Number or messages that can be stored in the queue at one time
#define PWNAGOTCHI_PROTOCOL_QUEUE_SIZE 5

/// Start byte at beginning of transmission
#define PWNAGOTCHI_PROTOCOL_START 0x02
/// End byte at the end of transmission
#define PWNAGOTCHI_PROTOCOL_END 0x03