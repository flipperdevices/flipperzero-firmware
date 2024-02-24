// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "DMComm.h"

namespace DMComm {

const uint8_t active_level = LOW;
const uint16_t sensor_threshold = 800;
const uint16_t pre_idle_send = 1000;
const uint16_t pre_active_min_ms = 100;
const uint16_t pre_active_send_ms = 150;
const uint16_t pre_active_max_ms = 200;
const uint16_t bit_idle_min = 200;
const uint16_t bit_idle_send = 500;
const uint16_t bit_idle_max = 700;
const uint16_t bit_active_min = 200;
const uint16_t bit0_active_send = 500;
const uint16_t bit_active_threshold = 1000;
const uint16_t bit1_active_send = 1500;
const uint16_t bit_active_max = 1700;
const uint16_t cooldown_send = 500;
const uint16_t reply_timeout_ms = 100;

ColorCommunicator::ColorCommunicator(BaseProngOutput& output, BaseProngInput& input) {
    output_ = &output;
    input_ = &input;
}

void ColorCommunicator::prepare(SignalType signal_type, uint16_t timeout_ms) {
    UNUSED(signal_type);
    initial_timeout_ms_ = timeout_ms;
    initial_receive_ = true;
    output_->begin();
    output_->setActiveLevel(active_level);
    input_->begin();
    input_->setActiveLevel(active_level);
    input_->setThreshold(sensor_threshold);
}

void ColorCommunicator::send(uint16_t data[], uint16_t length) {
    initial_receive_ = false;
    output_->driveIdle();
    delayMicroseconds(pre_idle_send);
    output_->driveActive();
    delay(pre_active_send_ms);
    for(uint16_t i = 0; i < length; i++) {
        uint16_t bits = data[i];
        for(int8_t b = 0; b < 16; b++) {
            if(bits & 1) {
                output_->driveIdle();
                delayMicroseconds(bit_idle_send);
                output_->driveActive();
                delayMicroseconds(bit1_active_send);
            } else {
                output_->driveIdle();
                delayMicroseconds(bit_idle_send);
                output_->driveActive();
                delayMicroseconds(bit0_active_send);
            }
            bits >>= 1;
        }
    }
    output_->driveIdle();
    delayMicroseconds(cooldown_send);
    output_->release();
}

ReceiveOutcome ColorCommunicator::receive(uint16_t buffer[], uint16_t buffer_size) {
    ReceiveOutcome outcome;
    uint32_t start_timeout = initial_receive_ ? initial_timeout_ms_ : reply_timeout_ms;
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
    outcome = input_->waitFrom(true, 1000L * pre_active_min_ms, 1000L * pre_active_max_ms, -2);
    if(outcome.status != kStatusReceived) {
        return outcome;
    }
    uint16_t result_length = 0;
    while(true) {
        uint16_t bits = 0;
        for(int8_t i = 0; i < 16; i++) {
            outcome = input_->waitFrom(false, bit_idle_min, bit_idle_max, i);
            if(outcome.status != kStatusReceived) {
                if(result_length > 0 && i == 0 && outcome.status == kErrorTimeout) {
                    outcome.status = kStatusReceived;
                }
                outcome.result_length = result_length;
                return outcome;
            }
            outcome = input_->waitFrom(true, bit_active_min, bit_active_max, i);
            if(outcome.status != kStatusReceived) {
                outcome.result_length = result_length;
                return outcome;
            }
            bits >>= 1;
            if(outcome.last_duration > bit_active_threshold) {
                bits |= 0x8000;
            }
        }
        if(result_length < buffer_size) {
            buffer[result_length] = bits;
            result_length++;
        } else {
            outcome.status = kErrorBufferFull;
            outcome.result_length = result_length;
            return outcome;
        }
    }
}

} // namespace DMComm
