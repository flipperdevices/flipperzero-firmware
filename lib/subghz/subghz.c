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
    SubGhzDecoderSerialization serialization;
    SubGhzSaveFile save_file;
} SubGhzDecoders;

typedef struct {
    SubGhzAlloc alloc;
    SubGhzEncodeLoad load;
    SubGhzEncoderStop stop;
    SubGhzFree free;
    SubGhzEncodeYield yield;
    SubGhzLoadFile load_file;
} SubGhzEncoders;

typedef struct {
    SubGhzEncoders encoder;
    SubGhzDecoders decoder;
    SubGhzProtocolSpecification protocol_spec;
} SubGhzEncoderDecoder;

static const SubGhzEncoderDecoder subghz_encoder_decoder[] = {
    {.decoder =
         {.alloc = subghz_protocol_came_decoder_alloc,
          .decode = subghz_protocol_came_decoder_feed,
          .reset = subghz_protocol_came_decoder_reset,
          .free = subghz_protocol_came_decoder_free,
          .serialization = subghz_protocol_came_decoder_serialization,
          .save_file = subghz_protocol_came_save_file},
     .encoder =
         {.alloc = subghz_protocol_came_encoder_alloc,
          .load = subghz_protocol_came_encoder_load,
          .stop = subghz_protocol_came_encoder_stop,
          .free = subghz_protocol_came_encoder_free,
          .yield = subghz_protocol_came_encoder_yield,
          .load_file = subghz_protocol_came_load_file},
     .protocol_spec =
         {
             .name = SUBGHZ_PROTOCOL_CAME_NAME,
             .type = SubGhzProtocolCommonTypeStatic_,
         }}

};

//decoder func
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

//registr callback decoder
//тут возможно перестарался
static void
    subghz_protocol_decoder_rx_callback(SubGhzProtocolBlockGeneric* generic, void* context) {
    SubGhzProtocolDecoder* instance = context;
    string_t protocol_name;
    string_init_printf(protocol_name, "%s", generic->protocol_name);
    if(instance->callback) {
        instance->callback(instance, instance->context, protocol_name);
    }
    string_clear(protocol_name);
}

void subghz_protocol_decoder_set_rx_callback(
    SubGhzProtocolDecoder* instance,
    SubGhzProtocolDecoderCallback callback,
    void* context) {
    furi_assert(instance);
    //???????????????                   ?
    for(int i = 0; i < COUNT_OF(subghz_encoder_decoder); ++i) {
        subghz_protocol_blocks_set_decoder_callback(
            (SubGhzProtocolBlockDecoder*)instance->protocol[i],
            subghz_protocol_decoder_rx_callback,
            instance);
    }
    instance->callback = callback;
    instance->context = context;
}

static int subghz_protocol_index_by_name(const char* protocol_name) {
    int result = -1;
    for(size_t i = 0; i < COUNT_OF(subghz_encoder_decoder); i++) {
        if(strcmp(subghz_encoder_decoder[i].protocol_spec.name, protocol_name) == 0) {
            result = i;
            break;
        }
    }
    return result;
}

SubGhzProtocolStatus subghz_protocol_decoder_serialization(
    SubGhzProtocolDecoder* instance,
    const char* protocol_name,
    string_t output) {
    SubGhzProtocolStatus status = SubGhzProtocolStatusUnknown;
    int index = subghz_protocol_index_by_name(protocol_name);
    if(index != SubGhzProtocolStatusNoProtocol) {
        subghz_encoder_decoder[index].decoder.serialization(instance->protocol[index], output);
        status = SubGhzProtocolStatusOk;
    } else {
        status = SubGhzProtocolStatusNoProtocol;
    }
    return status;
}

//encoder func
SubGhzProtocolEncoder* subghz_protocol_encoder_alloc_init(const char* protocol_name) {
    SubGhzProtocolEncoder* instance = NULL;
    int index = subghz_protocol_index_by_name(protocol_name);
    if(index != SubGhzProtocolStatusNoProtocol) {
        if(subghz_encoder_decoder[index].encoder.alloc) {
            instance = furi_alloc(sizeof(SubGhzProtocolEncoder));
            instance->handler = subghz_encoder_decoder[index].encoder.alloc();
            instance->index_protocol = index;
        }
    }
    // КАК вывести сообщение что нет акоего имени???
    return instance;
}

void subghz_protocol_encoder_free(SubGhzProtocolEncoder* instance) {
    if(instance) {
        free(instance->handler);
        free(instance);
    }
}

SubGhzProtocolStatus subghz_protocol_encoder_stop(SubGhzProtocolEncoder* instance) {
    SubGhzProtocolStatus status = SubGhzProtocolStatusNoInit;
    if(instance) {
        subghz_encoder_decoder[instance->index_protocol].encoder.stop(instance->handler);
        status = SubGhzProtocolStatusOk;
    }
    return status;
}

SubGhzProtocolStatus subghz_protocol_encoder_load(
    SubGhzProtocolEncoder* instance,
    uint64_t key,
    uint8_t count_bit,
    size_t repeat) {
    SubGhzProtocolStatus status = SubGhzProtocolStatusNoInit;
    if(instance) {
        subghz_encoder_decoder[instance->index_protocol].encoder.load(
            instance->handler, key, count_bit, repeat);
        status = SubGhzProtocolStatusOk;
    }
    return status;
}

LevelDuration subghz_protocol_encoder_yield(void* context) {
    SubGhzProtocolEncoder* instance = context;
    return subghz_encoder_decoder[instance->index_protocol].encoder.yield(instance->handler);
}
