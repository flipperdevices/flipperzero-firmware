// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "DMComm.h"

namespace DMComm {

void Controller::add(ClassicCommunicator& classic_comm) {
    classic_comm_ = &classic_comm;
}

void Controller::add(ColorCommunicator& color_comm) {
    color_comm_ = &color_comm;
}

void Controller::execute(BaseDigiROM& digirom, uint16_t listen_timeout_ms) {
    current_comm_ = nullptr;
    switch(digirom.signal_type()) {
    case kSignalTypeV:
    case kSignalTypeX:
    case kSignalTypeY:
        current_comm_ = classic_comm_;
        break;
    case kSignalTypeC:
        current_comm_ = color_comm_;
        break;
    default:
        break;
    }
    if(current_comm_ == nullptr) {
        // Either default above or relevant comm was not assigned.
        return; //TODO should we have some return values?
    }
    current_digirom_ = &digirom;
    digirom.prepare();
    current_comm_->prepare(digirom.signal_type(), listen_timeout_ms);
    if(digirom.turn() == 0) {
        while(receive())
            ;
    } else if(digirom.turn() == 1) {
        while(true) {
            if(!send()) {
                return;
            }
            if(!receive()) {
                return;
            }
        }
    } else if(digirom.turn() == 2) {
        while(true) {
            if(!receive()) {
                return;
            }
            if(!send()) {
                delay(DMCOMM_AFTER_RECEIVE_MILLIS);
                return;
            }
        }
    }
}

bool Controller::send() {
    int16_t length = current_digirom_->next(buffer_, DMCOMM_SIGNAL_BUFFER_SIZE);
    if(length <= 0) {
        return false;
    }
    current_comm_->send(buffer_, length);
    return true;
}

bool Controller::receive() {
    ReceiveOutcome outcome = current_comm_->receive(buffer_, DMCOMM_SIGNAL_BUFFER_SIZE);
    current_digirom_->store(buffer_, outcome);
    return outcome.status == kStatusReceived;
}

} // namespace DMComm
