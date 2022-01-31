#include "registry.h"
#include "came.h"

#define SUBGHZ_PARSER_TAG "SubGhzProtocol"

SubGhzProtocolDecoder subghz_protocol_came_decoder = {
    .alloc = subghz_protocol_decoder_came_alloc,
    .decode = subghz_protocol_decoder_came_feed,
    .reset = subghz_protocol_decoder_came_reset,
    .free = subghz_protocol_decoder_came_free,
    .save_file = subghz_protocol_came_save_file};

SubGhzProtocolEncoder subghz_protocol_came_encoder = {
    .alloc = subghz_protocol_encoder_came_alloc,
    .load = subghz_protocol_encoder_came_load,
    .stop = subghz_protocol_encoder_came_stop,
    .free = subghz_protocol_encoder_came_free,
    .yield = subghz_protocol_encoder_came_yield,
    .load_file = subghz_protocol_came_load_file};

const SubGhzProtocol subghz_protocol_registry[] = {{
    .specification =
        {
            .name = SUBGHZ_PROTOCOL_CAME_NAME,
            .type = SubGhzProtocolCommonTypeStatic_,
        },
    .decoder = &subghz_protocol_came_decoder,
    .encoder = &subghz_protocol_came_encoder,
}};

const SubGhzProtocol* subghz_protocol_registry_get_by_name(const char* name) {
    for(size_t i = 0; i < subghz_protocol_registry_count(); i++) {
        if(strcmp(name, subghz_protocol_registry[i].specification.name) == 0) {
            return &subghz_protocol_registry[i];
        }
    }
    return NULL;
}

const SubGhzProtocol* subghz_protocol_registry_get_by_index(size_t index) {
    if(index < subghz_protocol_registry_count()) {
        return &subghz_protocol_registry[index];
    } else {
        return NULL;
    }
}

size_t subghz_protocol_registry_count() {
    return COUNT_OF(subghz_protocol_registry);
}
