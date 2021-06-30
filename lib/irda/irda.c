#include "irda.h"
#include "furi/check.h"
#include "irda_common_i.h"
#include "irda_protocol_defs_i.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <furi.h>
#include "irda_i.h"


struct IrdaHandler {
    void** ctx;
};

typedef struct {
    IrdaAlloc alloc;
    IrdaDecode decode;
    IrdaReset reset;
    IrdaFree free;
} IrdaDecoders;

typedef struct {
    IrdaAlloc alloc;
    IrdaEncode encode;
    IrdaReset reset;
    IrdaFree free;
} IrdaEncoders;

typedef struct {
    IrdaProtocol protocol;
    const char* name;
    IrdaDecoders decoder;
    IrdaEncoders encoder;
    IrdaTimingsGet get_timings;
    uint8_t address_length;
    uint8_t command_length;
} IrdaProtocolImplementation;


// TODO: replace with key-value, Now we refer by enum index, which is dangerous.
static const IrdaProtocolImplementation irda_protocols[] = {
    // #3
    { .protocol = IrdaProtocolRC6,
      .name = "RC6",
      .decoder = {
          .alloc = irda_decoder_rc6_alloc,
          .decode = irda_decoder_rc6_decode,
          .reset = irda_decoder_rc6_reset,
          .free = irda_decoder_rc6_free},
      .encoder = {
          .encode = irda_encoder_rc6_encode,
          .alloc = irda_encoder_rc6_alloc,
          .reset = irda_encoder_rc6_reset,
          .free = irda_encoder_rc6_free},
      .address_length = 2,
      .command_length = 2,
    },
    // #0
    { .protocol = IrdaProtocolSamsung32,
      .name ="Samsung32",
      .decoder = {
          .alloc = irda_decoder_samsung32_alloc,
          .decode = irda_decoder_samsung32_decode,
          .reset = irda_decoder_samsung32_reset,
          .free = irda_decoder_samsung32_free},
//      .encoder = {
//          .encode = irda_encoder_samsung32_encode},
      .address_length = 2,
      .command_length = 2,
    },
    // #1
    { .protocol = IrdaProtocolNEC,
      .name = "NEC",
      .decoder = {
          .alloc = irda_decoder_nec_alloc,
          .decode = irda_decoder_nec_decode,
          .reset = irda_decoder_nec_reset,
          .free = irda_decoder_nec_free},
//      .encoder = {
//          .encode = irda_encoder_nec_encode},
      .address_length = 2,
      .command_length = 2,
    },
    // #2 - have to be after NEC
    { .protocol = IrdaProtocolNECext,
      .name = "NECext",
      .decoder = {
          .alloc = irda_decoder_necext_alloc,
          .decode = irda_decoder_nec_decode,
          .reset = irda_decoder_nec_reset,
          .free = irda_decoder_nec_free},
//      .encoder = {
//          .encode = irda_encoder_necext_encode},
      .address_length = 4,
      .command_length = 2,
    },
};


const IrdaMessage* irda_decode(IrdaHandler* handler, bool level, uint32_t duration) {
    furi_assert(handler);

    IrdaMessage* message = NULL;
    IrdaMessage* result = NULL;

    for (int i = 0; i < COUNT_OF(irda_protocols); ++i) {
        if (irda_protocols[i].decoder.decode) {
            message = irda_protocols[i].decoder.decode(handler->ctx[i], level, duration);
            if (!result && message) {
                message->protocol = irda_protocols[i].protocol;
                result = message;
            }
        }
    }

    return result;
}

IrdaHandler* irda_alloc_decoder(void) {
    IrdaHandler* handler = furi_alloc(sizeof(IrdaHandler));
    handler->ctx = furi_alloc(sizeof(void*) * COUNT_OF(irda_protocols));

    for (int i = 0; i < COUNT_OF(irda_protocols); ++i) {
        handler->ctx[i] = 0;
        if (irda_protocols[i].decoder.alloc)
            handler->ctx[i] = irda_protocols[i].decoder.alloc();
    }

    return handler;
}

void irda_free_decoder(IrdaHandler* handler) {
    furi_assert(handler);
    furi_assert(handler->ctx);

    for (int i = 0; i < COUNT_OF(irda_protocols); ++i) {
        if (irda_protocols[i].decoder.free)
            irda_protocols[i].decoder.free(handler->ctx[i]);
    }

    free(handler->ctx);
    free(handler);
}

void irda_reset_decoder(IrdaHandler* handler) {
    for (int i = 0; i < COUNT_OF(irda_protocols); ++i) {
        if (irda_protocols[i].decoder.reset)
            irda_protocols[i].decoder.reset(handler->ctx[i]);
    }
}

void irda_send_raw(const uint32_t timings[], uint32_t timings_cnt, bool start_from_mark, IrdaEncoderTimings* timings_settings) {
    IrdaEncoderTimings timings_settings_common = {
        .duty_cycle = IRDA_COMMON_DUTY_CYCLE,
        .carrier_frequency = IRDA_COMMON_CARRIER_FREQUENCY,
    };

    if (timings_settings == NULL) {
        timings_settings = &timings_settings_common;
    }

    for (uint32_t i = 0; i < timings_cnt; ++i) {
        if ((i % 2) ^ start_from_mark)
            irda_encode_mark(timings_settings, timings[i]);
        else
            irda_encode_space(timings_settings, timings[i]);
    }
    irda_encode_space(timings_settings, 0);
}

void irda_send(const IrdaMessage* message, int times) {
    furi_assert(message);
    furi_assert(irda_is_protocol_valid(message->protocol));
    furi_assert(irda_protocols[message->protocol].encoder.encode);

    const IrdaProtocolImplementation* protocol = &irda_protocols[message->protocol];
    void* encoder = protocol->encoder.alloc();

    __disable_irq();

    while (times) {
        IrdaStatus status;
        uint32_t duration = 0;
        bool level = false;
        status = protocol->encoder.encode(encoder, &duration, &level);
        if (level)
            irda_encode_mark(protocol->get_timings(), duration);
        else
            irda_encode_space(protocol->get_timings(), duration);
        if (status == IrdaStatusDone)
            --times;
    }

    IrdaEncoderTimings timings_settings_common = {
        .duty_cycle = IRDA_COMMON_DUTY_CYCLE,
        .carrier_frequency = IRDA_COMMON_CARRIER_FREQUENCY,
    };

    irda_encode_space(timings_settings, 0);

    __enable_irq();
}

void irda_send_rc6_plz(const IrdaMessage* message, int times) {
    IrdaCommonEncoder* encoder = irda_encoder_rc6_alloc();

    IrdaStatus status = IrdaStatusError;

    irda_encoder_rc6_free(encoder);
}

bool irda_is_protocol_valid(IrdaProtocol protocol) {
    return (protocol >= 0) && (protocol < COUNT_OF(irda_protocols));
}

IrdaProtocol irda_get_protocol_by_name(const char* protocol_name) {
    for (int i = 0; i < COUNT_OF(irda_protocols); ++i) {
        if (!strcmp(irda_protocols[i].name, protocol_name))
            return i;
    }
    return IrdaProtocolUnknown;
}

const char* irda_get_protocol_name(IrdaProtocol protocol) {
    if (irda_is_protocol_valid(protocol))
        return irda_protocols[protocol].name;
    else
        return "Invalid";
}

uint8_t irda_get_protocol_address_length(IrdaProtocol protocol) {
    if (irda_is_protocol_valid(protocol))
        return irda_protocols[protocol].address_length;
    else
        return 0;
}

uint8_t irda_get_protocol_command_length(IrdaProtocol protocol) {
    if (irda_is_protocol_valid(protocol))
        return irda_protocols[protocol].command_length;
    else
        return 0;
}

