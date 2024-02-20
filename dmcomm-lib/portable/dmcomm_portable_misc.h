// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_PORTABLE_MISC_H_
#define DMCOMM_PORTABLE_MISC_H_

#include <stdint.h>

namespace DMComm {

/**
 * The signal type for communicating with the toy.
 * V for 2-prong, X for 3-prong, Y for Xros Mini.
 */
enum SignalType {kSignalTypeV = 0, kSignalTypeX = 1, kSignalTypeY = 2, kSignalTypeC, kSignalTypeInfo, kSignalTypeProngTest, kSignalTypeError};

enum ResultSegmentType {kDataSent, kDataReceived, kNoData};

enum ReceiveStatus {kStatusReceived, kStatusNothing, kErrorBufferFull, kErrorTooShort, kErrorTimeout};

struct ReceiveOutcome {
    ReceiveStatus status = kStatusNothing;
    uint16_t result_length = 0;
    uint32_t last_duration = 0;
    int16_t current_bit = 0;
    bool current_bit_active = false;
};

struct DigiROMType {
    SignalType signal_type;
    uint8_t turn;
    uint8_t data_start;
};

DigiROMType digiROMType(const char * digirom);

char toUpper(char ch);

/**
 * Return integer value of hex digit character, or -1 if not a hex digit.
 */
int8_t hex2val(int8_t hexdigit);

/**
 * Return uppercase hex digit character for lowest 4 bits of input byte.
 */
int8_t val2hex(int8_t value);

}  // namespace DMComm

#endif  // DMCOMM_PORTABLE_MISC_H_
