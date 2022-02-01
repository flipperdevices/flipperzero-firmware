#include "base.h"
#include "registry.h"

void subghz_protocol_decoder_base_set_decoder_callback(
    SubGhzProtocolDecoderBase* decoder_base,
    SubGhzProtocolDecoderBaseRxCallback callback,
    void* context) {
    decoder_base->callback = callback;
    decoder_base->context = context;
}

SubGhzProtocolStatus subghz_protocol_decoder_base_serialize(
    SubGhzProtocolDecoderBase* decoder_base,
    string_t output) {
    SubGhzProtocolStatus status = SubGhzProtocolStatusUnknown;

    const SubGhzProtocol* protocol = subghz_protocol_registry_get_by_name(decoder_base->name);
    if(protocol && protocol->decoder && protocol->decoder->serialization) {
        protocol->decoder->serialization(decoder_base, output);
        status = SubGhzProtocolStatusOk;
    }
    return status;
}
