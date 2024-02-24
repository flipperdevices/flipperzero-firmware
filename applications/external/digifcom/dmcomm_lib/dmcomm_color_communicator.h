// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_COLOR_COMMUNICATOR_H_
#define DMCOMM_COLOR_COMMUNICATOR_H_

#include "DMComm.h"

namespace DMComm {

class ColorCommunicator : public BaseCommunicator {
public:
    ColorCommunicator(BaseProngOutput& output, BaseProngInput& input);
    void prepare(SignalType signal_type, uint16_t timeout_ms);
    void send(uint16_t data[], uint16_t length);
    ReceiveOutcome receive(uint16_t buffer[], uint16_t buffer_size);

private:
    BaseProngOutput* output_;
    BaseProngInput* input_;
    uint16_t initial_timeout_ms_;
    bool initial_receive_;
};

} // namespace DMComm

#endif // DMCOMM_COLOR_COMMUNICATOR_H_
