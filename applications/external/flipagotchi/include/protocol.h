#pragma once

#include <furi.h>

/// Number of bytes that can be stored in the queue at one time
#define PWNAGOTCHI_PROTOCOL_QUEUE_SIZE 5000

/// Max bytes of argument data
#define PWNAGOTCHI_PROTOCOL_ARGS_MAX 100

/// Start byte at beginning of transmission
#define PWNAGOTCHI_PROTOCOL_START 0x02
/// End byte at the end of transmission
#define PWNAGOTCHI_PROTOCOL_END 0x03

typedef struct {
    /// Parameter to operate on
    uint8_t parameterCode;

    /// Holds arguments sent folowing parameter
    uint8_t arguments[PWNAGOTCHI_PROTOCOL_ARGS_MAX];
} PwnCommand;
