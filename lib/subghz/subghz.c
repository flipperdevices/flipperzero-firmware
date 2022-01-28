#include "subghz.h"
#include "subghz_i.h"
#include "subghz_keystore.h"

#include <furi.h>
#include <m-string.h>

#include "subghz_protocol_blocks.h"

#include "protocols/subghz_protocol_came.h"

#define SUBGHZ_PARSER_TAG "SubGhzProtocol"

typedef struct {
    SubGhzAlloc alloc;
    SubGhzDecode decode;
    SubGhzDecoderReset reset;
    SubGhzFree free;
} SubGhzDecoders;

typedef struct {
    SubGhzAlloc alloc;
    SubGhzEncode encode;
    SubGhzEncoderReset reset;
    SubGhzFree free;
} SubGhzEncoders;

typedef struct {
    SubGhzEncoders encoder;
    SubGhzDecoders decoder;
    SubGhzProtocolSpecification protocol_spec;
} SubGhzEncoderDecoder;

static const SubGhzEncoderDecoder subghz_encoder_decoder[] = {
    {.decoder =
         {.alloc = subghz_protocol_came_decoder_alloc,
          .decode = subghz_protocol_came_decoder_decode,
          .reset = subghz_protocol_came_decoder_reset,
          .free = subghz_protocol_came_decoder_free},
     .encoder = {.alloc = NULL, .encode = NULL, .reset = NULL, .free = NULL},
     .protocol_spec =
         {
             .name = SUBGHZ_PROTOCOL_CAME_NAME,
             .type = SubGhzProtocolCommonTypeStatic,
         }}

};

SubGhzProtocolDecoder* subghz_protocol_decoder_alloc(void) {
    SubGhzProtocolDecoder* instance = furi_alloc(sizeof(SubGhzProtocolDecoder));
    instance->protocol = furi_alloc(sizeof(void*) * COUNT_OF(subghz_encoder_decoder));

    for(int i = 0; i < COUNT_OF(subghz_encoder_decoder); ++i) {
        instance->protocol[i] = NULL;

        if(subghz_encoder_decoder[i].decoder.alloc)
            instance->protocol[i] = subghz_encoder_decoder[i].decoder.alloc();
    }
    instance->callback = NULL;
    instance->context = NULL;
    return instance;
}

void subghz_protocol_decoder_free(SubGhzProtocolDecoder* instance) {
    furi_assert(instance);
    furi_assert(instance->protocol);

    for(int i = 0; i < COUNT_OF(subghz_encoder_decoder); ++i) {
        if(subghz_encoder_decoder[i].decoder.free)
            subghz_encoder_decoder[i].decoder.free(instance->protocol[i]);
    }
    instance->callback = NULL;
    instance->context = NULL;
    free(instance->protocol);
    free(instance);
}

void subghz_protocol_decoder_decode(SubGhzProtocolDecoder* instance, bool level, uint32_t duration) {
    furi_assert(instance);
    furi_assert(instance->protocol);

    for(int i = 0; i < COUNT_OF(subghz_encoder_decoder); ++i) {
        if(subghz_encoder_decoder[i].decoder.decode)
            subghz_encoder_decoder[i].decoder.decode(instance->protocol[i], level, duration);
    }
}

void subghz_protocol_decoder_reset(SubGhzProtocolDecoder* instance) {
    furi_assert(instance);
    furi_assert(instance->protocol);

    for(int i = 0; i < COUNT_OF(subghz_encoder_decoder); ++i) {
        if(subghz_encoder_decoder[i].decoder.reset)
            subghz_encoder_decoder[i].decoder.reset(instance->protocol[i]);
    }
}

//тут возможно перестарался
static void
    subghz_protocol_decoder_rx_callback(SubGhzProtocolDecoderRxCallback* decoder, void* context) {
    SubGhzProtocolBlockDecoder* instance = context;
    if(instance->callback) {
        instance->callback(decoder, instance->context);
    }
}

void subghz_protocol_decoder_set_callback(
    SubGhzProtocolDecoder* instance,
    SubGhzProtocolDecoderCallback callback,
    void* context) {
    furi_assert(instance);
    //???????????????                   ?
    for(int i = 0; i < COUNT_OF(subghz_encoder_decoder); ++i) {
        subghz_protocol_blocks_set_decoder_callback(
            instance->protocol[i], subghz_protocol_decoder_rx_callback, instance);
    }

    instance->callback = callback;
    instance->context = context;
}

// void* subghz_parser_get_by_name(const char* name) {
//     void* result = NULL;

//     for(size_t i = 0; i < COUNT_OF(subghz_encoder_decoder); i++) {
//         if(strcmp(subghz_encoder_decoder[i].protocol_spec.name, name) == 0) {
//             result = subghz_encoder_decoder[i];
//             break;
//         }
//     }

//     return result;
// }