// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_BASE_COMMUNICATOR_H_
#define DMCOMM_BASE_COMMUNICATOR_H_

#include "DMComm.h"

namespace DMComm {

class BaseCommunicator {
public:
    virtual ~BaseCommunicator() {
    }
    virtual void prepare(SignalType signal_type, uint16_t timeout_ms) = 0;
    virtual void send(uint16_t data[], uint16_t length) = 0;
    virtual ReceiveOutcome receive(uint16_t buffer[], uint16_t buffer_size) = 0;
};

} // namespace DMComm

#endif // DMCOMM_BASE_COMMUNICATOR_H_
