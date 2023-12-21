#include "calculation_logic.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "calculator.h"

// global constant lookup table
static const char* HEX_TO_BINARY_TABLE[16] = {
    "0000", "0001", "0010", "0011",
    "0100", "0101", "0110", "0111",
    "1000", "1001", "1010", "1011",
    "1100", "1101", "1110", "1111"
};

bool decToBin(const char* decString, char* binaryResult, size_t resultSize) {
    if (decString == NULL || binaryResult == NULL || resultSize < 2) {
        return false; // Invalid pointers or insufficient result size
    }

    char* end;
    unsigned long num = strtoul(decString, &end, 10);

    if (*end != '\0' || *decString == '\0') {
        return false; // Invalid decimal
    }

    // Calculate the number of bits
    size_t bitsNeeded = 0;
    unsigned long tempNum = num;
    while (tempNum > 0) {
        bitsNeeded++;
        tempNum >>= 1;
    }

    // If the number is zero
    if (num == 0) {
        if (resultSize < 2) {
            return false; // buffer
        }
        strcpy(binaryResult, "0");
        return true;
    }

    // check buffer
    if (bitsNeeded >= resultSize) {
        return false;
    }

    binaryResult[bitsNeeded] = '\0'; // Null-terminate the result

    // fill the binary string from the end
    for (int i = bitsNeeded - 1; i >= 0; i--) {
        binaryResult[i] = (num & 1) + '0'; // the least significant bit
        num >>= 1;
    }

    return true;
}

bool decToHex(const char* decString, char* hexResult, size_t resultSize) {
    if (decString == NULL || hexResult == NULL || resultSize == 0) {
        return false;
    }

    char* end;
    unsigned long num = strtoul(decString, &end, 10);

    if (*end != '\0' || *decString == '\0') {
        return false;
    }

    // buffer size
    size_t requiredSize = 1;
    unsigned long tempNum = num;

    while (tempNum >= 16) {
        requiredSize++;
        tempNum /= 16;
    }

    if (requiredSize + 1 > resultSize) {
        return false;
    }

    hexResult[requiredSize] = '\0';

    // convert to hexadecimal in reverse order
    do {
        int remainder = num % 16;
        hexResult[--requiredSize] = (remainder < 10) ? ('0' + remainder) : ('A' + (remainder - 10));
        num /= 16;
    } while (num > 0);

    return true;
}

bool decToChar(const char* decString, char* outChar) {
    if (decString == NULL || outChar == NULL || *decString == '\0') {
        return false;
    }

    char* end;
    unsigned long num = strtoul(decString, &end, 10);

    if (*end != '\0' || num > 255) {
        return false;
    }

    *outChar = (char)num;
    return true;
}

bool hexDigitToBinary(char hexDigit, char* binary) {
    const char* hexToBinary = NULL;

    if (hexDigit >= '0' && hexDigit <= '9') {
        hexToBinary = HEX_TO_BINARY_TABLE[hexDigit - '0'];
    } else if (hexDigit >= 'A' && hexDigit <= 'F') {
        hexToBinary = HEX_TO_BINARY_TABLE[10 + hexDigit - 'A'];
    } else if (hexDigit >= 'a' && hexDigit <= 'f') {
        hexToBinary = HEX_TO_BINARY_TABLE[10 + hexDigit - 'a'];
    }

    if (hexToBinary) {
        memcpy(binary, hexToBinary, 5);
        return true;
    } else {
        return false;
    }
}

bool hexToBin(const char* hexString, char* binaryResult, size_t resultSize) {
    if (hexString == NULL || binaryResult == NULL || resultSize == 0) {
        return false;
    }

    size_t requiredSize = strlen(hexString) * 4 + 1;
    if (requiredSize > resultSize) {
        return false;
    }

    while (*hexString) {
        const char* hexToBinary;
        if (*hexString >= '0' && *hexString <= '9') {
            hexToBinary = HEX_TO_BINARY_TABLE[*hexString - '0'];
        } else if (*hexString >= 'A' && *hexString <= 'F') {
            hexToBinary = HEX_TO_BINARY_TABLE[10 + *hexString - 'A'];
        } else if (*hexString >= 'a' && *hexString <= 'f') {
            hexToBinary = HEX_TO_BINARY_TABLE[10 + *hexString - 'a'];
        } else {
            return false;
        }

        if (resultSize < 4) {
            return false;
        }
        memcpy(binaryResult, hexToBinary, 4);
        binaryResult += 4;
        hexString++;
        resultSize -= 4;
    }

    *binaryResult = '\0';
    return true;
}

bool hexToDec(const char* hexString, int* outNum) {
    if (hexString == NULL || outNum == NULL) {
        return false;
    }

    *outNum = 0;
    while (*hexString) {
        char digit = *hexString;
        int value;

        if (digit >= '0' && digit <= '9') {
            value = digit - '0';
        } else if (digit >= 'A' && digit <= 'F') {
            value = digit - 'A' + 10;
        } else if (digit >= 'a' && digit <= 'f') {
            value = digit - 'a' + 10;
        } else {
            return false;
        }

        if (*outNum > INT_MAX / 16 || (*outNum == INT_MAX / 16 && value > INT_MAX % 16)) {
            return false; // check overflow
        }

        *outNum = *outNum * 16 + value;
        hexString++;
    }

    return true;
}

bool binToDec(const char* binaryString, int* decResult) {
    if (binaryString == NULL || decResult == NULL) {
        return false;
    }

    *decResult = 0;
    for (const char* p = binaryString; *p; ++p) {
        if (*p != '0' && *p != '1') {
            return false;
        }
        if (*decResult > INT_MAX / 2) {
            return false; // check overflow
        }
        *decResult = (*decResult << 1) | (*p - '0');
    }

    return true;
}

char binaryToHexDigit(const char* bin) {
    int value = 0;
    for (int i = 0; i < 4; ++i) {
        value = (value << 1) | (bin[i] - '0');
    }

    return (value < 10) ? ('0' + value) : ('A' + (value - 10));
}

bool binToHex(const char* binaryString, char* hexResult, size_t resultSize) {
    if (binaryString == NULL || hexResult == NULL || resultSize == 0) {
        return false;
    }

    size_t binLength = strlen(binaryString);

    for (size_t i = 0; i < binLength; ++i) {
        if (binaryString[i] != '0' && binaryString[i] != '1') {
            snprintf(hexResult, resultSize, "Invalid Binary");
            return false;
        }
    }

    // padding zeros - len multiple of 4
    size_t paddingZeros = (4 - (binLength % 4)) % 4;
    size_t totalChunks = (binLength + paddingZeros) / 4;

    if (totalChunks > resultSize - 1) {
        return false;
    }

    size_t hexIndex = 0;
    char tempBin[5] = "0000";
    size_t tempIndex = 0;

    for (size_t i = 0; i < binLength + paddingZeros; ++i) {
        if (i < paddingZeros) {
            tempBin[tempIndex++] = '0';
        } else {
            tempBin[tempIndex++] = binaryString[i - paddingZeros];
        }

        if (tempIndex == 4) {
            hexResult[hexIndex++] = binaryToHexDigit(tempBin);
            tempIndex = 0;
        }
    }

    hexResult[hexIndex] = '\0';
    return true;
}

void calculate(Calculator* calculator_state) {
    if (calculator_state == NULL) {
        return;
    }

    int num = 0;
    char result = '\0';

    switch (calculator_state->mode) {
        case ModeDecToBin:
            if (!decToBin(calculator_state->text, calculator_state->binaryResult, sizeof(calculator_state->binaryResult))) {
                snprintf(calculator_state->binaryResult, sizeof(calculator_state->binaryResult), "Invalid Dec");
            }
            break;

        case ModeDecToHex:
            if (!decToHex(calculator_state->text, calculator_state->hexResult, sizeof(calculator_state->hexResult))) {
                snprintf(calculator_state->hexResult, sizeof(calculator_state->hexResult), "Invalid Dec");
            }
            break;

        case ModeDecToChar:
            if (decToChar(calculator_state->text, &result)) {
                calculator_state->charResult[0] = result;
                calculator_state->charResult[1] = '\0';
            } else {
                snprintf(calculator_state->charResult, sizeof(calculator_state->charResult), "Invalid Dec");
            }
            break;

        case ModeHexToBin:
            if (!hexToBin(calculator_state->text, calculator_state->binaryResult, sizeof(calculator_state->binaryResult))) {
                snprintf(calculator_state->binaryResult, sizeof(calculator_state->binaryResult), "Invalid Hex");
            }
            break;

        case ModeHexToDec:
            if (hexToDec(calculator_state->text, &num)) {
                snprintf(calculator_state->decResult, sizeof(calculator_state->decResult), "%d", num);
            } else {
                snprintf(calculator_state->decResult, sizeof(calculator_state->decResult), "Invalid Hex");
            }
            break;

        case ModeBinToDec:
            if (binToDec(calculator_state->text, &num)) {
                snprintf(calculator_state->decResult, sizeof(calculator_state->decResult), "%d", num);
            } else {
                snprintf(calculator_state->decResult, sizeof(calculator_state->decResult), "Invalid Binary");
            }
            break;

        case ModeBinToHex:
            if (!binToHex(calculator_state->text, calculator_state->hexResult, sizeof(calculator_state->hexResult))) {
                snprintf(calculator_state->hexResult, sizeof(calculator_state->hexResult), "Invalid Binary");
            }
            break;

        default:
            break;
    }
}