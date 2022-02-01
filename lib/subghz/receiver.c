#include "receiver.h"
#include "protocols/registry.h"

SubGhzReceiver* subghz_receiver_alloc(void) {
    SubGhzReceiver* instance = furi_alloc(sizeof(SubGhzReceiver));
    SubGhzReceiverSlotArray_init(instance->slots);

    for(size_t i = 0; i < subghz_protocol_registry_count(); ++i) {
        const SubGhzProtocol* protocol = subghz_protocol_registry_get_by_index(i);

        if(protocol->decoder && protocol->decoder->alloc) {
            SubGhzReceiverSlot* slot = SubGhzReceiverSlotArray_push_new(instance->slots);
            slot->protocol = protocol;
            slot->protocol_instance = protocol->decoder->alloc();
        }
    }

    instance->callback = NULL;
    instance->context = NULL;

    return instance;
}

void subghz_receiver_free(SubGhzReceiver* instance) {
    furi_assert(instance);

    instance->callback = NULL;
    instance->context = NULL;

    // Release allocated slots
    for
        M_EACH(item, instance->slots, SubGhzReceiverSlotArray_t) {
            item->protocol->decoder->free(item->protocol_instance);
            item->protocol = NULL;
            item->protocol_instance = NULL;
        }
    SubGhzReceiverSlotArray_clear(instance->slots);

    free(instance);
}

void subghz_receiver_decode(SubGhzReceiver* instance, bool level, uint32_t duration) {
    furi_assert(instance);
    furi_assert(instance->slots);

    for
        M_EACH(item, instance->slots, SubGhzReceiverSlotArray_t) {
            item->protocol->decoder->decode(item->protocol_instance, level, duration);
        }
}

void subghz_receiver_reset(SubGhzReceiver* instance) {
    furi_assert(instance);
    furi_assert(instance->slots);

    for
        M_EACH(item, instance->slots, SubGhzReceiverSlotArray_t) {
            item->protocol->decoder->reset(item->protocol_instance);
        }
}

static void subghz_receiver_rx_callback(SubGhzProtocolDecoderBase* decoder_base, void* context) {
    SubGhzReceiver* instance = context;
    if(instance->callback) {
        instance->callback(instance, decoder_base, instance->context);
    }
}

void subghz_receiver_set_rx_callback(
    SubGhzReceiver* instance,
    SubGhzReceiverCallback callback,
    void* context) {
    furi_assert(instance);

    for
        M_EACH(item, instance->slots, SubGhzReceiverSlotArray_t) {
            subghz_protocol_decoder_base_set_decoder_callback(
                (SubGhzProtocolDecoderBase*)item->protocol_instance,
                subghz_receiver_rx_callback,
                instance);
        }

    instance->callback = callback;
    instance->context = context;
}
