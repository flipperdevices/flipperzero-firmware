// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "DMComm.h"

#define DMCOMM_CONF(name) dmcomm_conf::name[signal_type_]

namespace dmcomm_conf {
static const uint16_t active_level[] = {LOW, LOW, HIGH};
static const uint16_t invert_bit_read[] = {false, false, true};
static const uint16_t sensor_threshold[] = {1900, 1900, 800};
static const uint16_t pre_idle_send[] = {3000, 3000, 5000};
static const uint16_t pre_active_min_ms[] = {40, 40, 30};
static const uint16_t pre_active_send_ms[] = {59, 60, 40};
static const uint16_t pre_active_max_ms[] = {80, 80, 50};
static const uint16_t start_idle_min[] = {1400, 1500, 9000};
static const uint16_t start_idle_send[] = {2083, 2200, 11000};
static const uint16_t start_idle_max[] = {2500, 3500, 13000};
static const uint16_t start_active_min[] = {300, 1000, 4000};
static const uint16_t start_active_send[] = {917, 1600, 6000};
static const uint16_t start_active_max[] = {1400, 2600, 8000};
static const uint16_t bit_idle_min[] = {400, 800, 800};
static const uint16_t bit0_idle_send[] = {1000, 1600, 4000};
static const uint16_t bit_idle_threshold[] = {1800, 2600, 3000};
static const uint16_t bit1_idle_send[] = {2667, 4000, 1400};
static const uint16_t bit_idle_max[] = {3400, 5500, 4500};
static const uint16_t bit_active_min[] = {800, 1000, 1000};
static const uint16_t bit1_active_send[] = {1667, 1600, 4400};
static const uint16_t bit0_active_send[] = {3167, 4000, 1600};
static const uint16_t bit_active_max[] = {4000, 5500, 5000};
static const uint16_t cooldown_send[] = {400, 400, 200};
static const uint16_t reply_timeout_ms[] = {100, 100, 100};
} // namespace dmcomm_conf

namespace DMComm {

ClassicCommunicator::ClassicCommunicator(BaseProngOutput& output, BaseProngInput& input) {
    output_ = &output;
    input_ = &input;
    signal_type_ = kSignalTypeV;
}

void ClassicCommunicator::prepare(SignalType signal_type, uint16_t timeout_ms) {
    signal_type_ = signal_type;
    initial_timeout_ms_ = timeout_ms;
    initial_receive_ = true;
    uint8_t active_level = (uint8_t)DMCOMM_CONF(active_level);
    output_->begin();
    output_->setActiveLevel(active_level);
    input_->begin();
    input_->setActiveLevel(active_level);
    input_->setThreshold(DMCOMM_CONF(sensor_threshold));
}

void ClassicCommunicator::send(uint16_t data[], uint16_t length) {
    UNUSED(length);
    uint16_t bits = data[0];
    initial_receive_ = false;
    output_->driveIdle();
    delayMicroseconds(DMCOMM_CONF(pre_idle_send));
    output_->driveActive();
    delay(DMCOMM_CONF(pre_active_send_ms));
    output_->driveIdle();
    delayMicroseconds(DMCOMM_CONF(start_idle_send));
    output_->driveActive();
    delayMicroseconds(DMCOMM_CONF(start_active_send));
    for(int8_t i = 0; i < 16; i++) {
        if(bits & 1) {
            output_->driveIdle();
            delayMicroseconds(DMCOMM_CONF(bit1_idle_send));
            output_->driveActive();
            delayMicroseconds(DMCOMM_CONF(bit1_active_send));
        } else {
            output_->driveIdle();
            delayMicroseconds(DMCOMM_CONF(bit0_idle_send));
            output_->driveActive();
            delayMicroseconds(DMCOMM_CONF(bit0_active_send));
        }
        bits >>= 1;
    }
    output_->driveIdle();
    delayMicroseconds(DMCOMM_CONF(cooldown_send));
    output_->release();
}

ReceiveOutcome ClassicCommunicator::receive(uint16_t buffer[], uint16_t buffer_size) {
    UNUSED(buffer_size);
    ReceiveOutcome outcome;
    uint16_t bits = 0;
    uint32_t start_timeout = initial_receive_ ? initial_timeout_ms_ :
                                                DMCOMM_CONF(reply_timeout_ms);
    start_timeout *= 1000;
    if(initial_receive_) {
        outcome = input_->waitFrom(true, 0, start_timeout, -3);
        if(outcome.status == kErrorTimeout) {
            outcome.status = kStatusNothing;
            return outcome;
        }
        start_timeout -= outcome.last_duration;
    }
    outcome = input_->waitFrom(false, 0, start_timeout, -2);
    if(outcome.status == kErrorTimeout) {
        outcome.status = kStatusNothing;
        return outcome;
    }
    initial_receive_ = false;
    outcome = input_->waitFrom(
        true, 1000L * DMCOMM_CONF(pre_active_min_ms), 1000L * DMCOMM_CONF(pre_active_max_ms), -2);
    if(outcome.status != kStatusReceived) {
        if(outcome.status == kErrorTimeout && signal_type_ == kSignalTypeY) {
            // Xros Mini disconnected on D-Com, ignore
            outcome.status = kStatusNothing;
        }
        return outcome;
    }
    outcome =
        input_->waitFrom(false, DMCOMM_CONF(start_idle_min), DMCOMM_CONF(start_idle_max), -1);
    if(outcome.status != kStatusReceived) {
        return outcome;
    }
    outcome =
        input_->waitFrom(true, DMCOMM_CONF(start_active_min), DMCOMM_CONF(start_active_max), -1);
    if(outcome.status != kStatusReceived) {
        return outcome;
    }
    for(int8_t i = 0; i < 16; i++) {
        outcome = input_->waitFrom(false, DMCOMM_CONF(bit_idle_min), DMCOMM_CONF(bit_idle_max), i);
        if(outcome.status != kStatusReceived) {
            return outcome;
        }
        bits >>= 1;
        if(outcome.last_duration > DMCOMM_CONF(bit_idle_threshold)) {
            bits |= 0x8000;
        }
        outcome =
            input_->waitFrom(true, DMCOMM_CONF(bit_active_min), DMCOMM_CONF(bit_active_max), i);
        if(outcome.status != kStatusReceived) {
            if(outcome.status == kErrorTimeout && signal_type_ == kSignalTypeX && i == 15) {
                // iC bug, ignore
                outcome.status = kStatusReceived;
            } else {
                return outcome;
            }
        }
    }
    if(DMCOMM_CONF(invert_bit_read)) {
        bits ^= 0xFFFF;
    }
    buffer[0] = bits;
    outcome.result_length = 1;
    return outcome;
}

} // namespace DMComm
