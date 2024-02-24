// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "dmcomm_portable_misc.h"

namespace DMComm {

DigiROMType digiROMType(const char* digirom) {
    uint8_t chunk_len = 0;
    while(digirom[chunk_len] != '\0' && digirom[chunk_len] != '-' && digirom[chunk_len] != ' ' &&
          chunk_len < 4) {
        chunk_len++;
    }
    char op1 = '_';
    char op2 = '_';
    char turn_ch = '_';
    switch(chunk_len) {
    case 1:
        op1 = digirom[0];
        break;
    case 2:
        op1 = digirom[0];
        turn_ch = digirom[1];
        break;
    case 3:
        op1 = digirom[0];
        op2 = digirom[1];
        turn_ch = digirom[2];
        break;
    }
    bool has_turn = turn_ch >= '0' && turn_ch <= '2';
    SignalType signal_type = kSignalTypeError;
    op1 = toUpper(op1);
    op2 = toUpper(op2);
    if(op1 == 'V' && op2 == '_' && has_turn) {
        signal_type = kSignalTypeV;
    } else if(op1 == 'X' && op2 == '_' && has_turn) {
        signal_type = kSignalTypeX;
    } else if(op1 == 'Y' && op2 == '_' && has_turn) {
        signal_type = kSignalTypeY;
    } else if(op1 == 'C' && op2 == '_' && has_turn) {
        signal_type = kSignalTypeC;
    } else if(op1 == 'I' && op2 == '_' && !has_turn) {
        signal_type = kSignalTypeInfo;
    } else if(op1 == 'T' && op2 == '_' && !has_turn) {
        signal_type = kSignalTypeProngTest;
    }
    uint8_t turn;
    if(signal_type == kSignalTypeError) {
        turn = 0;
        chunk_len = 0;
    } else {
        turn = has_turn ? (turn_ch - '0') : 0;
    }
    return {signal_type, turn, chunk_len};
}

char toUpper(char ch) {
    if(ch >= 'a' && ch <= 'z') {
        return (ch - 0x20);
    }
    return ch;
}

int8_t hex2val(int8_t hexdigit) {
    int8_t value;
    if(hexdigit >= '0' && hexdigit <= '9') {
        value = hexdigit - 0x30;
    } else if(hexdigit >= 'a' && hexdigit <= 'f') {
        value = hexdigit - 0x57;
    } else if(hexdigit >= 'A' && hexdigit <= 'F') {
        value = hexdigit - 0x37;
    } else {
        value = -1;
    }
    return value;
}

int8_t val2hex(int8_t value) {
    value &= 0xF;
    if(value > 9) {
        return value + 0x37;
    } else {
        return value + 0x30;
    }
}

} // namespace DMComm
