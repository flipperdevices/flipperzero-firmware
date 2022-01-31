#include "base.h"

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

    // int index = subghz_protocol_index_by_instance(protocol_name);
    // if(index != SubGhzProtocolStatusNoProtocol) {
    //     subghz_encoder_decoder[index].decoder.serialization(instance->protocol[index], output);
    //     status = SubGhzProtocolStatusOk;
    // } else {
    //     status = SubGhzProtocolStatusNoProtocol;
    // }

    return status;
}
