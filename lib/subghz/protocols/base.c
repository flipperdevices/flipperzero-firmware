#include "base.h"
#include "registry.h"

void subghz_protocol_decoder_base_set_decoder_callback(
    SubGhzProtocolDecoderBase* decoder_base,
    SubGhzProtocolDecoderBaseRxCallback callback,
    void* context) {
    furi_check(decoder_base);

    decoder_base->callback = callback;
    decoder_base->context = context;
}

bool subghz_protocol_decoder_base_get_string(
    SubGhzProtocolDecoderBase* decoder_base,
    FuriString* output) {
    furi_check(decoder_base);
    furi_check(output);

    bool status = false;

    if(decoder_base->protocol && decoder_base->protocol->decoder &&
       decoder_base->protocol->decoder->get_string) {
        decoder_base->protocol->decoder->get_string(decoder_base, output);
        status = true;
    }

    return status;
}

SubGhzProtocolStatus subghz_protocol_decoder_base_serialize(
    SubGhzProtocolDecoderBase* decoder_base,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset) {
    furi_check(decoder_base);
    furi_check(flipper_format);

    SubGhzProtocolStatus status = SubGhzProtocolStatusError;

    if(decoder_base->protocol && decoder_base->protocol->decoder &&
       decoder_base->protocol->decoder->serialize) {
        status = decoder_base->protocol->decoder->serialize(decoder_base, flipper_format, preset);
    }

    return status;
}

SubGhzProtocolStatus subghz_protocol_decoder_base_deserialize(
    SubGhzProtocolDecoderBase* decoder_base,
    FlipperFormat* flipper_format) {
    furi_check(decoder_base);

    SubGhzProtocolStatus status = SubGhzProtocolStatusError;

    if(decoder_base->protocol && decoder_base->protocol->decoder &&
       decoder_base->protocol->decoder->deserialize) {
        status = decoder_base->protocol->decoder->deserialize(decoder_base, flipper_format);
    }

    return status;
}

uint8_t subghz_protocol_decoder_base_get_hash_data(SubGhzProtocolDecoderBase* decoder_base) {
    furi_check(decoder_base);

    uint8_t hash = 0;
    if(!decoder_base->protocol || !decoder_base->protocol->decoder) return hash;

    if(decoder_base->protocol->decoder->get_hash_data) {
        hash = decoder_base->protocol->decoder->get_hash_data(decoder_base);
    } else if(decoder_base->protocol->decoder->get_hash_data_long) {
        uint32_t long_hash = decoder_base->protocol->decoder->get_hash_data_long(decoder_base);
        uint8_t* p = (uint8_t*)&long_hash;
        for(size_t i = 0; i < sizeof(long_hash); i++) {
            hash ^= p[i];
        }
    }

    return hash;
}

uint32_t subghz_protocol_decoder_base_get_hash_data_long(SubGhzProtocolDecoderBase* decoder_base) {
    uint32_t hash = 0;
    if(!decoder_base->protocol || !decoder_base->protocol->decoder) return hash;

    if(decoder_base->protocol->decoder->get_hash_data) {
        hash = decoder_base->protocol->decoder->get_hash_data(decoder_base);
    } else if(decoder_base->protocol->decoder->get_hash_data_long) {
        hash = decoder_base->protocol->decoder->get_hash_data_long(decoder_base);
    }

    return hash;
}
