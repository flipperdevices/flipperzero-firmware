// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_H_
#define DMCOMM_H_

#include "arduino.h"

#define DMCOMM_NO_PIN 0xFF
#define DMCOMM_SIGNAL_TIMED_OUT 0xFFFFFFFF
#define DMCOMM_SIGNAL_BUFFER_SIZE 8
#define DMCOMM_SERIAL_TIMEOUT_MILLIS 6000
#define DMCOMM_GOFIRST_BEFORE_MILLIS 1000
#define DMCOMM_GOFIRST_REPEAT_MILLIS 5000
#define DMCOMM_LISTEN_TIMEOUT_MILLIS 5000
#define DMCOMM_INACTIVE_DELAY_MILLIS 3000
#define DMCOMM_AFTER_RECEIVE_MILLIS 300

namespace DMComm {

/**
 * Call pinMode unless pin is DMCOMM_NO_PIN.
 */
void pinModeMaybe(uint8_t pin, uint8_t mode);

/**
 * Call digitalWrite unless pin is DMCOMM_NO_PIN.
 */
void digitalWriteMaybe(uint8_t pin, uint8_t val);

} // namespace DMComm

#ifndef DMCOMM_BUILD_INFO
#include "dmcomm_build_info.h"
#endif

#include "portable/dmcomm_printonly.h"
#include "dmcomm_pin_control.h"
#include "dmcomm_rp2040.h"
#include "dmcomm_prong_test.h"
#include "dmcomm_base_communicator.h"
#include "dmcomm_classic_communicator.h"
#include "dmcomm_color_communicator.h"
#include "dmcomm_led.h"
#include "dmcomm_controller.h"
#include "dmcomm_serial.h"

#endif // DMCOMM_H_
