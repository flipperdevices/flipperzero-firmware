#include "base.h"
#include "registry.h"

void subghz_protocol_decoder_base_set_decoder_callback(
    SubGhzProtocolDecoderBase* decoder_base,
    SubGhzProtocolDecoderBaseRxCallback callback,
    void* context) {
    decoder_base->callback = callback;
    decoder_base->context = context;
}

bool subghz_protocol_decoder_base_serialize(
    SubGhzProtocolDecoderBase* decoder_base,
    string_t output) {
    bool status = false;

    if(decoder_base->protocol && decoder_base->protocol->decoder &&
       decoder_base->protocol->decoder->serialize) {
        decoder_base->protocol->decoder->serialize(decoder_base, output);
        status = true;
    }

    return status;
}
