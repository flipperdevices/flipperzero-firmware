// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "DMComm.h"

#define DMCOMM_INDICATE_NEW 0
#define DMCOMM_INDICATE_NEW_ERROR 1
#define DMCOMM_INDICATE_EXECUTED 2

namespace DMComm {

SerialFollower::SerialFollower(Controller& controller, Stream& serial)
    : controller_(controller)
    , serial_(serial) {
}

SerialFollower::~SerialFollower() {
    delete_digirom();
}

void SerialFollower::setIndicator(Indicator& indicator) {
    indicator_ = &indicator;
    indicator_->begin();
}

void SerialFollower::setProngTester(ProngTester& prong_tester) {
    prong_tester_ = &prong_tester;
}

void SerialFollower::loop() {
    uint8_t i = serialRead();
    if(i > 0) {
        if(strncmp(command_buffer_, "ready", 5) ==
           0) { // Hack to make Alpha Serial (windows) detect this as an A-Com
            serial_.println(F("ready"));
            return;
        }
        DigiROMType rom_type = digiROMType(command_buffer_);
        if(rom_type.signal_type != kSignalTypeInfo &&
           rom_type.signal_type != kSignalTypeProngTest) {
            serial_.print(F("got "));
            serial_.print(i, DEC);
            serial_.print(F(" bytes: "));
            serial_.write(command_buffer_, i);
            serial_.print(F(" -> "));
        }
        delete_digirom();
        switch(rom_type.signal_type) {
        case kSignalTypeV:
        case kSignalTypeX:
        case kSignalTypeY:
            digirom_ = new ClassicDigiROM(command_buffer_);
            break;
        case kSignalTypeC:
            digirom_ = new WordsDigiROM(command_buffer_);
            break;
        default:
            break;
        }
        if(rom_type.signal_type == kSignalTypeInfo) {
            serial_.print(DMCOMM_BUILD_INFO);
        } else if(rom_type.signal_type == kSignalTypeProngTest) {
            serial_.println(F("[info]"));
            serial_.println(DMCOMM_BUILD_INFO);
            if(prong_tester_ != nullptr) {
                prong_tester_->run(serial_);
            }
        } else if(digirom_ != nullptr) {
            indicate(DMCOMM_INDICATE_NEW);
            serial_.print(F("(new DigiROM)"));
        } else {
            indicate(DMCOMM_INDICATE_NEW_ERROR);
            serial_.print(F("(paused)"));
        }
        serial_.println();
        if(digirom_ != nullptr && digirom_->turn() == 1) {
            delay(DMCOMM_GOFIRST_BEFORE_MILLIS);
        }
    }
    if(digirom_ != nullptr) {
        controller_.execute(*digirom_);
        indicate(DMCOMM_INDICATE_EXECUTED);
        digirom_->printResult(serial_);
        serial_.println();
        if(digirom_->turn() == 1) {
            delay(DMCOMM_GOFIRST_REPEAT_MILLIS);
        }
    } else {
        delay(DMCOMM_INACTIVE_DELAY_MILLIS);
    }
}

uint8_t SerialFollower::serialRead() {
    uint32_t time_start;
    uint32_t time;
    int16_t incoming_int;
    uint8_t incoming_byte;
    uint8_t i = 0;
    bool first = true;
    if(serial_.available() == 0) {
        return 0;
    }
    time_start = millis();
    do {
        do {
            incoming_int = serial_.read();
            // if we have nothing to read, and this is the first character
            // then exit immediately, this makes our loop really tight while we are active
            if(first && incoming_int == -1) {
                return 0;
            }
            first = false;
            time = millis() - time_start;
            if(time > DMCOMM_SERIAL_TIMEOUT_MILLIS) {
                serial_.println(F("[No EOL received]"));
                return 0;
            }
        } while(incoming_int == -1);
        incoming_byte = incoming_int;
        if(incoming_byte != '\r' && incoming_byte != '\n') {
            command_buffer_[i] = incoming_byte;
            i += 1;
        }
    } while(incoming_byte != '\r' && incoming_byte != '\n' && i < DMCOMM_TEXT_DIGIROM_SIZE - 1);
    if(incoming_byte != '\r' && incoming_byte != '\n') {
        serial_.println(F("[DigiROM too long]"));
        while(serial_.read() != -1)
            ; // empty the buffer
        return 0;
    }
    command_buffer_[i] = '\0';
    return i;
}

void SerialFollower::delete_digirom() {
    if(digirom_ != nullptr) {
        delete digirom_;
        digirom_ = nullptr;
    }
}

void SerialFollower::indicate(uint8_t type) {
    if(indicator_ == nullptr) {
        return;
    }
    switch(type) {
    case DMCOMM_INDICATE_NEW:
        indicator_->new_digirom(digirom_);
        break;
    case DMCOMM_INDICATE_NEW_ERROR:
        indicator_->new_digirom_error();
        break;
    case DMCOMM_INDICATE_EXECUTED:
        indicator_->executed_digirom(digirom_);
        break;
    }
}

} // namespace DMComm
