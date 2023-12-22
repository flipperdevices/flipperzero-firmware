#include "calculator_state.h"
#include "calculator.h"

void toggle_mode(Calculator* calculator_state) {
    switch (calculator_state->mode) {
        case ModeDecToBin:
            calculator_state->mode = ModeDecToHex;
            break;
        case ModeDecToHex:
            calculator_state->mode = ModeDecToChar;
            break;
        case ModeDecToChar:
            calculator_state->mode = ModeHexToBin;
            break;
        case ModeHexToBin:
            calculator_state->mode = ModeHexToDec;
            break;
        case ModeHexToDec:
            calculator_state->mode = ModeBinToDec;
            break;
        case ModeBinToDec:
            calculator_state->mode = ModeBinToHex;
            break;
        case ModeBinToHex:
            calculator_state->mode = ModeDecToBin;
            break;
        default:
            calculator_state->mode = ModeDecToBin;
    }
}
