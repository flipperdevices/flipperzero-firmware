#include "calculation_logic.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "calculator.h"

// Global constant lookup table
const char* HEX_TO_BINARY_TABLE[16] = {
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
    unsigned long num = strtoul(decString, &end, 10); // positive numbers only

    if (*end != '\0' || *decString == '\0') {
        return false; // Invalid decimal
    }

    // Calculate the number of bits required for the binary representation
    size_t bitsNeeded = 0;
    unsigned long tempNum = num;
    while (tempNum > 0) {
        bitsNeeded++;
        tempNum >>= 1;
    }

    // If the number is zero
    if (num == 0) {
        if (resultSize < 2) {
            return false; // Check buffer size
        }
        strcpy(binaryResult, "0");
        return true;
    }

    // Ensure the buffer is large enough to store the binary representation
    if (bitsNeeded >= resultSize) {
        return false; // Not enough space in result buffer
    }

    binaryResult[bitsNeeded] = '\0'; // Null-terminate the result

    // Fill the binary string from the end
    for (int i = bitsNeeded - 1; i >= 0; i--) {
        binaryResult[i] = (num & 1) + '0'; // Get the least significant bit
        num >>= 1;
    }

    return true;
}

bool decToHex(const char* decString, char* hexResult, size_t resultSize) {
    char* end;
    long num = strtol(decString, &end, 10);

    if (*end != '\0' || num < 0) {
        return false; // Invalid decimal number
    }

    if (num == 0) {
        strncpy(hexResult, "0", resultSize);
        return true;
    }

    char hex[17]; // Maximum 16 hex digits + null terminator
    int index = 0;

    while (num > 0 && index < 16) {
        int remainder = num % 16;
        if (remainder < 10) {
            hex[index++] = '0' + remainder;
        } else {
            hex[index++] = 'A' + (remainder - 10);
        }
        num /= 16;
    }

    hex[index] = '\0'; // Null-terminate

    // Reverse the hex string
    for (int i = 0; i < index / 2; i++) {
        char temp = hex[i];
        hex[i] = hex[index - 1 - i];
        hex[index - 1 - i] = temp;
    }

    strncpy(hexResult, hex, resultSize - 1);
    hexResult[resultSize - 1] = '\0';

    return true;
}

bool decToChar(const char* decString, char* outChar) {
    char* end;
    long num = strtol(decString, &end, 10);

    // Check for valid decimal input and range for a character
    if (*end != '\0' || num < 0 || num > 255) {
        return false; // Invalid input or out of char range
    }

    *outChar = (char)num;
    return true;
}

// Function to convert a single hex digit to binary
void hexDigitToBinary(char hexDigit, char* binary) {
    if (hexDigit >= '0' && hexDigit <= '9') {
        strcpy(binary, HEX_TO_BINARY_TABLE[hexDigit - '0']);
    } else if (hexDigit >= 'A' && hexDigit <= 'F') {
        strcpy(binary, HEX_TO_BINARY_TABLE[10 + hexDigit - 'A']);
    } else if (hexDigit >= 'a' && hexDigit <= 'f') {
        strcpy(binary, HEX_TO_BINARY_TABLE[10 + hexDigit - 'a']);
    } else {
        strcpy(binary, "????");
    }
}

bool hexToBin(const char* hexString, char* binaryResult, size_t resultSize) {
    size_t hexLength = strlen(hexString);
    size_t binaryIndex = 0;

    for (size_t i = 0; i < hexLength; i++) {
        char hexDigit = hexString[i];
        char binaryDigit[5]; // 4 binary digits + null terminator

        hexDigitToBinary(hexDigit, binaryDigit);

        // Check for invalid hex digit
        if (strcmp(binaryDigit, "????") == 0) {
            return false;
        }

        // Ensure enough space in result
        if (binaryIndex + 4 >= resultSize) {
            return false;
        }

        strcpy(&binaryResult[binaryIndex], binaryDigit);
        binaryIndex += 4;
    }

    binaryResult[binaryIndex] = '\0'; // Null-terminate the result
    return true;
}

bool hexToDec(const char* hexString, int* outNum) {
    *outNum = 0;
    bool validHex = true;

    for (; *hexString && validHex; hexString++) {
        char digit = *hexString;
        if (digit >= '0' && digit <= '9') {
            *outNum = *outNum * 16 + (digit - '0');
        } else if (digit >= 'A' && digit <= 'F') {
            *outNum = *outNum * 16 + (digit - 'A' + 10);
        } else if (digit >= 'a' && digit <= 'f') {
            *outNum = *outNum * 16 + (digit - 'a' + 10);
        } else {
            validHex = false;
        }
    }

    return validHex;
}

bool binToDec(const char* binaryString, int* decResult) {
    size_t len = strlen(binaryString);
    *decResult = 0;
    for (size_t i = 0; i < len; ++i) {
        char bit = binaryString[i];
        if (bit != '0' && bit != '1') {
            return false; // Invalid binary digit
        }
        *decResult = (*decResult << 1) | (bit - '0');
    }
    return true;
}

char binaryToHexDigit(const char* bin) {
    for (int i = 0; i < 16; ++i) {
        if (strcmp(bin, HEX_TO_BINARY_TABLE[i]) == 0) {
            return (i < 10) ? ('0' + i) : ('A' + (i - 10));
        }
    }
    return '?'; // Invalid binary input
}

bool binToHex(const char* binaryString, char* hexResult, size_t resultSize) {
    size_t binLength = strlen(binaryString);
    size_t hexIndex = 0;

    // Validate the binary string and calculate the number of leading zeros needed
    int leadingZeros = (4 - (binLength % 4)) % 4;
    for (size_t i = 0; i < binLength; ++i) {
        if (binaryString[i] != '0' && binaryString[i] != '1') {
            snprintf(hexResult, resultSize, "Invalid Binary");
            return false; // Invalid character in binary string
        }
    }

    char tempBin[5] = {0};

    // Add leading zeros if necessary
    for (int i = 0; i < leadingZeros; ++i) {
        tempBin[i] = '0';
    }

    // Process the leading bits if there were any leading zeros
    if (leadingZeros > 0) {
        strncpy(tempBin + leadingZeros, binaryString, 4 - leadingZeros);
        hexResult[hexIndex++] = binaryToHexDigit(tempBin);
        binaryString += 4 - leadingZeros; // Move the pointer ahead
        binLength -= 4 - leadingZeros;
    }

    // Process the rest of the binary string in 4-bit chunks
    for (size_t i = 0; i < binLength; i += 4) {
        if (hexIndex >= resultSize - 1) {
            return false; // Buffer overflow check
        }
        strncpy(tempBin, binaryString + i, 4);
        hexResult[hexIndex++] = binaryToHexDigit(tempBin);
    }

    hexResult[hexIndex] = '\0'; // Null-terminate the result
    return true;
}









void calculate(Calculator* calculator_state) {
    int num;
    char result;
    if (calculator_state->mode == ModeDecToBin) {
        if (decToBin(calculator_state->text, calculator_state->binaryResult, sizeof(calculator_state->binaryResult))) {
            // Conversion successful, binary result is already in calculator_state->binaryResult
        } else {
            snprintf(calculator_state->binaryResult, sizeof(calculator_state->binaryResult), "Invalid Dec");
        }
    } else if (calculator_state->mode == ModeDecToHex) {
        if (decToHex(calculator_state->text, calculator_state->hexResult, sizeof(calculator_state->hexResult))) {
            // Conversion successful, hex result is already in calculator_state->hexResult
        } else {
            snprintf(calculator_state->hexResult, sizeof(calculator_state->hexResult), "Invalid Dec");
        }
    } else if (calculator_state->mode == ModeDecToChar) {
        if (decToChar(calculator_state->text, &result)) {
            // Conversion successful, store the character result
            calculator_state->charResult[0] = result;
            calculator_state->charResult[1] = '\0'; // Null-terminate the string
        } else {
            snprintf(calculator_state->charResult, sizeof(calculator_state->charResult), "Invalid Dec");
        }
    } else if (calculator_state->mode == ModeHexToBin) {
        if (hexToBin(calculator_state->text, calculator_state->binaryResult, sizeof(calculator_state->binaryResult))) {
            // Conversion successful, binary result is already in calculator_state->binaryResult
        } else {
            snprintf(calculator_state->binaryResult, sizeof(calculator_state->binaryResult), "Invalid Hex");
        }
    } else if (calculator_state->mode == ModeHexToDec) {
        if (hexToDec(calculator_state->text, &num)) {
            snprintf(calculator_state->decResult, sizeof(calculator_state->decResult), "%d", num);
        } else {
            snprintf(calculator_state->hexResult, sizeof(calculator_state->decResult), "Invalid Hex");
        }
    } else if (calculator_state->mode == ModeBinToDec) {
        int decResult;
        if (binToDec(calculator_state->text, &decResult)) {
            snprintf(calculator_state->decResult, sizeof(calculator_state->decResult), "%d", decResult);
        } else {
            snprintf(calculator_state->decResult, sizeof(calculator_state->decResult), "Invalid Binary");
        }
    }else if (calculator_state->mode == ModeBinToHex) {
        if (binToHex(calculator_state->text, calculator_state->hexResult, sizeof(calculator_state->hexResult))) {
            // Conversion successful, hex result is already in calculator_state->hexResult
        } else {
            snprintf(calculator_state->hexResult, sizeof(calculator_state->hexResult), "Invalid Binary");
        }
    }
}


