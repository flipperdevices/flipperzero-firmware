#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <furi.h>
#include "protocol_defs_i.h"
#include "irda_i.h"
#include "decoder_common_i.h"


struct IrdaHandler {
    void** ctx;
};

typedef struct {
    IrdaInit init;
    IrdaDecode decode;
    IrdaFini fini;
} IrdaDecoders;

typedef struct {
    IrdaEncode encode;
} IrdaEncoders;

typedef struct {
    IrdaProtocol protocol;
    const char* name;
    IrdaDecoders decoder;
    IrdaEncoders encoder;
} IrdaProtocolImplementation;


// TODO: replace with key-value, Now we refer by enum index, which is dangerous.
static const IrdaProtocolImplementation irda_protocols[] = {
    { IrdaProtocolSamsung32, "Samsung32",   {init_samsung32, decode_samsung32, fini_samsung32}, {encode_samsung32}  },
    { IrdaProtocolNEC,       "NEC",         {init_nec,       decode_nec,       fini_nec},       {encode_nec}        },
};


const IrdaMessage* irda_decode(IrdaHandler* handler, bool level, uint32_t duration) {
    furi_assert(handler);

    IrdaMessage* message = NULL;
    IrdaMessage* result = NULL;

    for (int i = 0; i < COUNT_OF(irda_protocols); ++i) {
        message = irda_protocols[i].decoder.decode(handler->ctx[i], level, duration);
        if (!result && message) {
            message->protocol = irda_protocols[i].protocol;
            result = message;
        }
    }

    return result;
}

IrdaHandler* irda_init_decoder(void) {
    IrdaHandler* handler = furi_alloc(sizeof(IrdaHandler));
    handler->ctx = furi_alloc(sizeof(void*) * COUNT_OF(irda_protocols));

    for (int i = 0; i < COUNT_OF(irda_protocols); ++i) {
        handler->ctx[i] = irda_protocols[i].decoder.init();
        furi_check(handler->ctx[i]);
    }

    return handler;
}

void irda_free_decoder(IrdaHandler* handler) {
    furi_assert(handler);
    furi_assert(handler->ctx);

    for (int i = 0; i < COUNT_OF(irda_protocols); ++i) {
        irda_protocols[i].decoder.fini(handler->ctx[i]);
    }

    free(handler->ctx);
    free(handler);
}

void irda_send(const IrdaMessage* message, int times) {
    furi_assert(message);

    for (int i = 0; i < times; ++i) {
        osKernelLock();
        __disable_irq();
        irda_protocols[message->protocol].encoder.encode(message->address, message->command, !!i);
        __enable_irq();
        osKernelUnlock();
    }
}

const char* irda_get_protocol_name(IrdaProtocol protocol) {
    return irda_protocols[protocol].name;
}

