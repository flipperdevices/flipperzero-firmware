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


static IrdaDecoders irda_decoders[] = {
    {init_nec,      decode_nec,     fini_nec},
};


const IrdaMessage* irda_decode(IrdaHandler *d, bool level, uint32_t duration) {
    const IrdaMessage* im = 0;

    for (int i = 0; i < COUNT_OF(irda_decoders); ++i) {
        im = irda_decoders[i].decode(d->ctx[i], level, duration);
        if (im)
            break;
    }

    return im;
}

IrdaHandler* irda_init_decoder(void) {
    IrdaHandler* d = malloc(sizeof(IrdaHandler));
    furi_assert(d);
    d->ctx = malloc(sizeof(void*) * COUNT_OF(irda_decoders));
    furi_assert(d->ctx);
    for (int i = 0; i < COUNT_OF(irda_decoders); ++i) {
        d->ctx[i] = irda_decoders[i].init();
        furi_assert(d->ctx[i]);
    }
    return d;
}

void irda_free_decoder(IrdaHandler* d) {
    free(d->ctx);
    free(d);
}

