#include "strint.h"

#include <string.h>

StrintParseError strint_to_uint32(char* str, char** end, uint32_t* out, uint8_t base) {
    // skip whitespace
    while(((*str >= 9) && (*str <= 13)) || *str == 32) {
        str++;
    }

    // read sign
    bool negative = false;
    if(*str == '+' || *str == '-') {
        if(*str == '-') negative = true;
        str++;
    }
    if(*str == '+' || *str == '-') return StrintParseSignError;
    if(negative) return StrintParseSignError; // TODO: int32_t

    // infer base
    if(base == 0) {
        if(memcmp(str, "0x", 2) == 0 || memcmp(str, "0X", 2) == 0) {
            base = 16;
            str += 2;
        } else if(memcmp(str, "0b", 2) == 0 || memcmp(str, "0B", 2) == 0) {
            base = 2;
            str += 2;
        } else if(*str == '0') {
            base = 8;
            str++;
        } else {
            base = 10;
        }
    }

    // read digits
    uint32_t result = 0;
    int read_total = 0;
    while(*str != 0) {
        int digit_value;
        if(*str >= '0' && *str <= '9') {
            digit_value = *str - '0';
        } else if(*str >= 'A' && *str <= 'Z') {
            digit_value = *str - 'A' + 10;
        } else if(*str >= 'a' && *str <= 'z') {
            digit_value = *str - 'a' + 10;
        } else {
            break;
        }

        if(digit_value >= base) {
            break;
        }

        if(result > UINT32_MAX / base) return StrintParseOverflowError;
        result *= base;
        if(result > UINT32_MAX - digit_value) return StrintParseOverflowError;
        result += digit_value;

        read_total++;
        str++;
    }

    if(read_total == 0) return StrintParseAbsentError;

    if(negative) result = -result;
    if(out) *out = result;
    if(end) *end = str;
    return StrintParseNoError;
}
