#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IrdaDecoder IrdaDecoder;
typedef struct IrdaProtocolSpec IrdaProtocolSpec;


typedef struct {
    uint32_t adr;
    uint32_t cmd;
    IrdaProtocolSpec* protocol;
    bool repeat;
} IrdaMessage;

IrdaDecoder* irda_new_decoder(void);
void irda_free_decoder(IrdaDecoder* d);
IrdaMessage* irda_decode(IrdaDecoder *d, bool level, uint32_t duration);
const char* irda_get_protocol_name(const IrdaProtocolSpec* protocol);

#ifdef __cplusplus
}
#endif

