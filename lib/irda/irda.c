#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <furi.h>
#include "decoder_nec_i.h"
#include "irda.h"


#define COUNT_OF(x)                     (sizeof(x) / sizeof(x[0]))


struct IrdaHandler {
    void** ctx;
};

typedef struct IrdaDecoders {
    Init init;
    Decode decode;
    Fini fini;
} IrdaDecoders;


static const IrdaDecoders irda_decoders[] = {
    {init_nec,      decode_nec,     fini_nec},
};


const IrdaMessage* irda_decode(IrdaHandler* handler, bool level, uint32_t duration) {
    furi_assert(handler);

    const IrdaMessage* message = 0;

    for (int i = 0; i < COUNT_OF(irda_decoders); ++i) {
        message = irda_decoders[i].decode(handler->ctx[i], level, duration);
        if (message)
            break;
    }

    return message;
}

IrdaHandler* irda_init_decoder(void) {
    IrdaHandler* handler = furi_alloc(sizeof(IrdaHandler));
    handler->ctx = furi_alloc(sizeof(void*) * COUNT_OF(irda_decoders));

    for (int i = 0; i < COUNT_OF(irda_decoders); ++i) {
        handler->ctx[i] = irda_decoders[i].init();
        furi_check(handler->ctx[i]);
    }
    return handler;
}

void irda_free_decoder(IrdaHandler* handler) {
    furi_assert(handler);
    furi_assert(handler->ctx);

    for (int i = 0; i < COUNT_OF(irda_decoders); ++i) {
        irda_decoders[i].fini(handler->ctx[i]);
    }

    free(handler->ctx);
    free(handler);
}

