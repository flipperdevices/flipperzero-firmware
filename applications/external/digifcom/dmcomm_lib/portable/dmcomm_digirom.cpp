// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "dmcomm_digirom.h"

namespace DMComm {

void printHex(Print& dest, uint16_t value, uint8_t num_digits) {
    const uint8_t max_digits = 4;
    int8_t i;
    uint8_t digits[max_digits];
    if(num_digits > max_digits) {
        num_digits = max_digits;
    }
    for(i = 0; i < num_digits; i++) {
        digits[i] = val2hex((uint8_t)value);
        value /= 0x10;
    }
    for(i = num_digits - 1; i >= 0; i--) {
        dest.write(digits[i]);
    }
}

void printReceiveOutcome(Print& dest, ReceiveOutcome outcome) {
    switch(outcome.status) {
    case kStatusReceived:
        dest.print(F("[Received]"));
        break;
    case kStatusNothing:
        dest.print(F("[Nothing]"));
        break;
    case kErrorBufferFull:
        dest.print(F("[Buffer full]"));
        break;
    case kErrorTooShort:
    case kErrorTimeout:
        dest.print(F("[Bit "));
        dest.print(outcome.current_bit);
        if(outcome.current_bit_active) {
            dest.print(F(" active "));
        } else {
            dest.print(F(" idle "));
        }
        if(outcome.status == kErrorTooShort) {
            dest.print(F("too short "));
            dest.print(outcome.last_duration);
            dest.write(']');
        } else {
            dest.print(F("timed out]"));
        }
        break;
    }
}

uint16_t BaseCore::length() {
    return length_;
}

bool BaseCore::somethingReceived() {
    for(uint16_t i = 0; i < length_; i++) {
        if(result_types_[i] == kDataReceived) {
            return true;
        }
    }
    return false;
}

void BaseCore::printResult(Print& dest) {
    for(uint16_t i = 0; i < length_; i++) {
        ResultSegmentType seg_type = result_types_[i];
        if(seg_type == kNoData) {
            dest.write('t');
        } else {
            if(seg_type == kDataSent) {
                dest.write('s');
            } else {
                dest.write('r');
            }
            dest.write(':');
            printResultSegmentData(dest, i);
        }
        dest.write(' ');
    }
    switch(last_outcome_.status) {
    case kStatusReceived:
    case kStatusNothing:
        break;
    default:
        printReceiveOutcome(dest, last_outcome_);
    }
}

uint16_t BaseDigiROM::resultSize() {
    return base_core_->length();
}

bool BaseDigiROM::somethingReceived() {
    return base_core_->somethingReceived();
}

void BaseDigiROM::printResult(Print& dest) {
    base_core_->printResult(dest);
}

void ClassicCore::prepare() {
    length_ = 0;
    checksum_ = 0;
}

uint16_t ClassicCore::next(
    uint16_t bits,
    uint16_t copy_mask,
    uint16_t invert_mask,
    int8_t checksum_target,
    uint8_t check_digit_LSB_pos) {
    uint16_t bits_received = 0;
    if(length_ > 0) {
        bits_received = result_data_[length_ - 1];
    }
    bits &= ~copy_mask;
    bits |= copy_mask & bits_received;
    bits &= ~invert_mask;
    bits |= invert_mask & ~bits_received;
    if(checksum_target >= 0) {
        bits &= ~(0xF << check_digit_LSB_pos);
    }
    for(int8_t i = 0; i < 4; i++) {
        checksum_ += bits >> (4 * i);
    }
    checksum_ %= 16;
    if(checksum_target >= 0) {
        uint16_t check_digit = (checksum_target - checksum_) % 16;
        bits |= check_digit << check_digit_LSB_pos;
        checksum_ = checksum_target;
    }
    result_append(kDataSent, bits);
    return bits;
}

void ClassicCore::store(uint16_t data[], ReceiveOutcome outcome) {
    last_outcome_ = outcome;
    if(outcome.status == kStatusReceived) {
        result_append(kDataReceived, data[0]);
    } else if(outcome.status == kStatusNothing) {
        result_append(kNoData, 0);
    }
}

void ClassicCore::printResultSegmentData(Print& dest, uint16_t index) {
    printHex(dest, result_data_[index], 4);
}

void ClassicCore::result_append(ResultSegmentType type, uint16_t data) {
    if(length_ < DMCOMM_CLASSIC_RESULT_SIZE) {
        result_types_[length_] = type;
        result_data_[length_] = data;
        length_++;
    }
}

void WordsCore::prepare() {
    length_ = 0;
}

void WordsCore::store_sent(uint16_t data[], uint16_t length) {
    result_append(kDataSent, data, length);
}

void WordsCore::store_received(uint16_t data[], ReceiveOutcome outcome) {
    last_outcome_ = outcome;
    if(outcome.status == kStatusReceived) {
        result_append(kDataReceived, data, outcome.result_length);
    } else if(outcome.status == kStatusNothing) {
        result_append(kNoData, data, 0);
    }
}

void WordsCore::printResultSegmentData(Print& dest, uint16_t index) {
    for(uint16_t j = 0; j < result_segments_[index].length; j++) {
        printHex(dest, result_segments_[index].data[j], 4);
    }
}

void WordsCore::result_append(ResultSegmentType type, uint16_t data[], uint16_t segment_length) {
    if(segment_length > DMCOMM_WORDS_RESULT_SEGMENT_SIZE) {
        segment_length = DMCOMM_WORDS_RESULT_SEGMENT_SIZE; //TODO ?
    }
    if(length_ < DMCOMM_WORDS_RESULT_SIZE) {
        result_types_[length_] = type;
        result_segments_[length_].length = segment_length;
        for(uint16_t i = 0; i < segment_length; i++) {
            result_segments_[length_].data[i] = data[i];
        }
        length_++;
    }
}

void BaseTextDigiROM::loadTextDigiROM(const char* digirom) {
    DigiROMType rom_type = digiROMType(digirom);
    signal_type_ = rom_type.signal_type;
    turn_ = rom_type.turn;
    data_start_ = rom_type.data_start;
    uint8_t i = 0;
    while(digirom[i] != '\0' && i < DMCOMM_TEXT_DIGIROM_SIZE - 1) {
        digirom_[i] = digirom[i];
        i++;
    }
    digirom_[i] = '\0';
}

SignalType BaseTextDigiROM::signal_type() {
    return signal_type_;
}

uint8_t BaseTextDigiROM::turn() {
    return turn_;
}

ClassicDigiROM::ClassicDigiROM(const char* digirom) {
    loadTextDigiROM(digirom);
    base_core_ = &core_;
}

void ClassicDigiROM::prepare() {
    core_.prepare();
    cursor_ = digirom_ + data_start_;
}

int16_t ClassicDigiROM::next(uint16_t buffer[], uint16_t buffer_size) {
    uint16_t bits = 0;
    uint16_t copy_mask = 0;
    uint16_t invert_mask = 0;
    int8_t checksum_target = -1;
    uint8_t check_digit_LSB_pos = 12;
    if(buffer_size < 1) {
        return -2;
    }
    if(*cursor_ == '\0' || *cursor_ == ' ') {
        return 0;
    }
    // Require first character to be dash
    if(*cursor_ != '-') {
        return -1;
    }
    cursor_++;
    for(int8_t i = 0; i < 4; i++) {
        uint8_t LSB_pos = 12 - (i * 4);
        bits <<= 4;
        char ch1 = *cursor_;
        char ch_digit = ch1;
        if(ch1 == '@' || ch1 == '^') {
            cursor_++;
            ch_digit = *cursor_;
            if(ch_digit == '\0') {
                cursor_--;
            }
        }
        int8_t digit = hex2val(ch_digit);
        if(digit < 0) {
            return -1;
        }
        if(ch1 == '@') {
            checksum_target = digit;
            check_digit_LSB_pos = LSB_pos;
        } else if(ch1 == '^') {
            copy_mask |= (~digit & 0xF) << LSB_pos;
            invert_mask |= digit << LSB_pos;
        } else {
            bits |= digit;
        }
        cursor_++;
    }
    buffer[0] = core_.next(bits, copy_mask, invert_mask, checksum_target, check_digit_LSB_pos);
    return 1;
}

void ClassicDigiROM::store(uint16_t data[], ReceiveOutcome outcome) {
    core_.store(data, outcome);
}

WordsDigiROM::WordsDigiROM(const char* digirom) {
    loadTextDigiROM(digirom);
    base_core_ = &core_;
}

void WordsDigiROM::prepare() {
    core_.prepare();
    cursor_ = digirom_ + data_start_;
}

int16_t WordsDigiROM::next(uint16_t buffer[], uint16_t buffer_size) {
    uint8_t length = 0;
    if(*cursor_ == '\0' || *cursor_ == ' ') {
        return 0;
    }
    // Require first character to be dash
    if(*cursor_ != '-') {
        return -1;
    }
    cursor_++;
    int8_t digit_count = 0;
    uint16_t bits = 0;
    while(true) {
        int8_t digit = hex2val(*cursor_);
        if(digit == -1) {
            if(digit_count == 0) {
                core_.store_sent(buffer, length);
                return length;
            } else {
                return -2;
            }
        }
        bits <<= 4;
        bits |= digit;
        digit_count++;
        digit_count %= 4;
        if(digit_count == 0) {
            if(length < buffer_size) {
                buffer[length] = bits;
                length++;
                bits = 0;
            } else {
                return -3;
            }
        }
        cursor_++;
    }
}

void WordsDigiROM::store(uint16_t data[], ReceiveOutcome outcome) {
    core_.store_received(data, outcome);
}

} // namespace DMComm
