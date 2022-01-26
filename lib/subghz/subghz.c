#include "subghz.h"
#include "subghz_i.h"
#include "subghz_keystore.h"

#include <furi.h>
#include <m-string.h>

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

struct SubGhzProtocolDecoder {
    void** protocol;
    SubGhzProtocolDecoderCallback callback;
    void* context;
};

struct SubGhzProtocolEncoderHandler {
    void* handler;
    const SubGhzEncoders* encoder;
};

static const SubGhzEncoderDecoder subghz_encoder_decoder[] = {
    {.decoder =
         {.alloc = (void*)subghz_protocol_came_alloc,
          .decode = subghz_protocol_came_parse,
          .reset = subghz_protocol_came_reset,
          .free = subghz_protocol_came_free},
     .encoder =
         {
             .alloc = NULL,
             .encode = NULL,
             .reset = NULL,
             .free = NULL,
         }}

};

SubGhzProtocolDecoder* subghz_protocol_alloc_decoder(void) {
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

void subghz_protocol_free_decoder(SubGhzProtocolDecoder* instance) {
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

void subghz_protocol_decoder_set_callback(
    SubGhzProtocolDecoder* instance,
    SubGhzProtocolDecoderCallback callback,
    void* context) {
    furi_assert(instance);
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