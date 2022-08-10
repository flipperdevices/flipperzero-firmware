#include <furi.h>
#include "protocol_dict.h"

struct ProtocolDict {
    const ProtocolBase** base;
    size_t count;
    void** data;
};

ProtocolDict* protocol_dict_alloc(const ProtocolBase** protocols, size_t count) {
    ProtocolDict* dict = malloc(sizeof(ProtocolDict));
    dict->base = protocols;
    dict->count = count;
    dict->data = malloc(sizeof(void*) * dict->count);

    for(size_t i = 0; i < dict->count; i++) {
        dict->data[i] = dict->base[i]->alloc();
    }

    return dict;
}

void protocol_dict_free(ProtocolDict* dict) {
    for(size_t i = 0; i < dict->count; i++) {
        dict->base[i]->free(dict->data[i]);
    }

    free(dict->data);
    free(dict);
}

void protocol_dict_set_data(
    ProtocolDict* dict,
    size_t protocol_index,
    const uint8_t* data,
    size_t data_size) {
    furi_assert(protocol_index < dict->count);
    ProtocolSetData set_data = dict->base[protocol_index]->set_data;

    if(set_data) {
        set_data(dict->data[protocol_index], data, data_size);
    }
}

void protocol_dict_get_data(
    ProtocolDict* dict,
    size_t protocol_index,
    uint8_t* data,
    size_t data_size) {
    furi_assert(protocol_index < dict->count);
    ProtocolGetData get_data = dict->base[protocol_index]->get_data;

    if(get_data) {
        get_data(dict->data[protocol_index], data, data_size);
    }
}

size_t protocol_dict_get_data_size(ProtocolDict* dict, size_t protocol_index) {
    furi_assert(protocol_index < dict->count);
    ProtocolGetDataSize get_data_size = dict->base[protocol_index]->get_data_size;
    size_t data_size = 0;

    if(get_data_size) {
        data_size = get_data_size(dict->data[protocol_index]);
    }

    return data_size;
}

size_t protocol_dict_get_max_data_size(ProtocolDict* dict) {
    size_t max_data_size = 0;
    for(size_t i = 0; i < dict->count; i++) {
        ProtocolGetDataSize get_data_size = dict->base[i]->get_data_size;

        if(get_data_size) {
            size_t data_size = get_data_size(dict->data[i]);
            if(data_size > max_data_size) {
                max_data_size = data_size;
            }
        }
    }

    return max_data_size;
}

const char* protocol_dict_get_name(ProtocolDict* dict, size_t protocol_index) {
    furi_assert(protocol_index < dict->count);
    ProtocolGetName get_name = dict->base[protocol_index]->get_name;
    const char* name = "Unknown";

    if(get_name) {
        name = get_name(dict->data[protocol_index]);
    }

    return name;
}

const char* protocol_dict_get_manufacturer(ProtocolDict* dict, size_t protocol_index) {
    furi_assert(protocol_index < dict->count);
    ProtocolGetManufacturer get_man = dict->base[protocol_index]->get_manufacturer;
    const char* name = "Unknown";

    if(get_man) {
        name = get_man(dict->data[protocol_index]);
    }

    return name;
}

void protocol_dict_decoders_start(ProtocolDict* dict) {
    for(size_t i = 0; i < dict->count; i++) {
        ProtocolDecoderStart fn = dict->base[i]->decoder.start;

        if(fn) {
            fn(dict->data[i]);
        }
    }
}

ProtocolId protocol_dict_decoders_feed(ProtocolDict* dict, bool level, uint32_t duration) {
    bool done = false;
    ProtocolId ready_protocol_id = PROTOCOL_NO;

    for(size_t i = 0; i < dict->count; i++) {
        ProtocolDecoderFeed fn = dict->base[i]->decoder.feed;

        if(fn) {
            if(fn(dict->data[i], level, duration)) {
                if(!done) {
                    ready_protocol_id = i;
                    done = true;
                }
            }
        }
    }

    return ready_protocol_id;
}

ProtocolId protocol_dict_decoders_feed_by_feature(
    ProtocolDict* dict,
    bool level,
    uint32_t duration,
    uint32_t feature) {
    bool done = false;
    ProtocolId ready_protocol_id = PROTOCOL_NO;

    for(size_t i = 0; i < dict->count; i++) {
        uint32_t features = dict->base[i]->get_features(dict->data[i]);
        if(features & feature) {
            ProtocolDecoderFeed fn = dict->base[i]->decoder.feed;

            if(fn) {
                if(fn(dict->data[i], level, duration)) {
                    if(!done) {
                        ready_protocol_id = i;
                        done = true;
                    }
                }
            }
        }
    }

    return ready_protocol_id;
}

bool protocol_dict_encoder_start(ProtocolDict* dict, size_t protocol_index) {
    furi_assert(protocol_index < dict->count);
    ProtocolEncoderStart fn = dict->base[protocol_index]->encoder.start;

    if(fn) {
        return fn(dict->data[protocol_index]);
    } else {
        return false;
    }
}

LevelDuration protocol_dict_encoder_yield(ProtocolDict* dict, size_t protocol_index) {
    furi_assert(protocol_index < dict->count);
    ProtocolEncoderYield fn = dict->base[protocol_index]->encoder.yield;

    if(fn) {
        return fn(dict->data[protocol_index]);
    } else {
        return level_duration_reset();
    }
}

void protocol_dict_render_data(ProtocolDict* dict, string_t result, size_t protocol_index) {
    furi_assert(protocol_index < dict->count);
    ProtocolRenderData fn = dict->base[protocol_index]->render_data;

    if(fn) {
        return fn(dict->data[protocol_index], result);
    }
}

uint32_t protocol_dict_get_validate_count(ProtocolDict* dict, size_t protocol_index) {
    furi_assert(protocol_index < dict->count);
    ProtocolGetValidateCount fn = dict->base[protocol_index]->get_validate_count;

    furi_assert(fn);
    return fn(dict->data[protocol_index]);
}

ProtocolId protocol_dict_get_protocol_by_name(ProtocolDict* dict, const char* name) {
    for(size_t i = 0; i < dict->count; i++) {
        if(strcmp(name, protocol_dict_get_name(dict, i)) == 0) {
            return i;
        }
    }
    return PROTOCOL_NO;
}