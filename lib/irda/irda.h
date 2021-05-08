#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IrdaHandler IrdaHandler;

// Do not change protocol order, as it can be saved into memory and fw update can be performed!
typedef enum {
    IrdaProtocolSamsung32 = 0,
    IrdaProtocolNEC = 1,
} IrdaProtocol;

typedef struct {
    IrdaProtocol protocol;
    uint32_t address;
    uint32_t command;
    bool repeat;
} IrdaMessage;

IrdaHandler* irda_init_decoder(void);
const IrdaMessage* irda_decode(IrdaHandler* handler, bool level, uint32_t duration);
void irda_free_decoder(IrdaHandler* handler);
void irda_send(const IrdaMessage* message, int times);
const char* irda_get_protocol_name(IrdaProtocol protocol);

#ifdef __cplusplus
}
#endif

