#include "irda-app.hpp"
#include "irda.h"
#include <api-hal-irda.h>

void IrdaAppSignalTranceiver::irda_rx_callback(void* ctx, bool level, uint32_t duration) {
    IrdaAppEvent event;
    const IrdaMessage* irda_message;
    IrdaAppSignalTranceiver* this_ = static_cast<IrdaAppSignalTranceiver*>(ctx);

    irda_message = irda_decode(this_->decoder, level, duration);
    if(irda_message) {
        this_->capture_stop();
        this_->message = *irda_message;
        event.type = IrdaAppEvent::Type::IrdaMessageReceived;
        osStatus_t result = osMessageQueuePut(this_->event_queue, &event, 0, 0);
        furi_check(result == osOK);
    }
}

IrdaAppSignalTranceiver::IrdaAppSignalTranceiver(void)
    : decoder(irda_alloc_decoder()) {
}

IrdaAppSignalTranceiver::~IrdaAppSignalTranceiver() {
    api_hal_irda_rx_irq_deinit();
    irda_free_decoder(decoder);
}

void IrdaAppSignalTranceiver::capture_once_start(osMessageQueueId_t queue) {
    event_queue = queue;
    irda_reset_decoder(decoder);
    api_hal_irda_rx_irq_init();
    api_hal_irda_rx_irq_set_callback(IrdaAppSignalTranceiver::irda_rx_callback, this);
}

void IrdaAppSignalTranceiver::capture_stop(void) {
    api_hal_irda_rx_irq_deinit();
}

IrdaMessage* IrdaAppSignalTranceiver::get_last_message(void) {
    return &message;
}

void IrdaAppSignalTranceiver::send_message(const IrdaMessage* message) const {
    irda_send(message, 1);
}
