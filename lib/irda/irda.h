#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct IrdaHandler IrdaHandler;

typedef struct {
    const char* protocol_name;
    uint32_t adr;
    uint32_t cmd;
    bool repeat;
} IrdaMessage;

typedef const IrdaMessage* (*Decode) (void* ctx, bool level, uint32_t duration);
typedef void* (*Init) (void);
typedef void (*Fini) (void*);


IrdaHandler* irda_init_decoder(void);
const IrdaMessage* irda_decode(IrdaHandler *d, bool level, uint32_t duration);
void irda_free_decoder(IrdaHandler * d);

#ifdef __cplusplus
}
#endif

