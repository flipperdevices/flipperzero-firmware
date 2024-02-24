// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_CONTROLLER_H_
#define DMCOMM_CONTROLLER_H_

#include "DMComm.h"

namespace DMComm {

class Controller {
public:
    void add(ClassicCommunicator& classic_comm);
    void add(ColorCommunicator& color_comm);
    void execute(BaseDigiROM& digirom, uint16_t listen_timeout_ms = DMCOMM_LISTEN_TIMEOUT_MILLIS);

private:
    uint16_t buffer_[DMCOMM_SIGNAL_BUFFER_SIZE];
    ClassicCommunicator* classic_comm_ = nullptr;
    ColorCommunicator* color_comm_ = nullptr;
    BaseCommunicator* current_comm_;
    BaseDigiROM* current_digirom_;
    bool send();
    bool receive();
};

} // namespace DMComm

#endif // DMCOMM_CONTROLLER_H_
