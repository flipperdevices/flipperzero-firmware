#include "strint.h"

#include <string.h>

// Splitting out the actual parser helps reduce code size. The manually
// monomorphized `strint_to_*`s are just wrappers around this generic
// implementation.
/**
 * @brief Converts a string to a `uint64_t` and an auxillary sign bit, checking
 * the bounds of the integer.
 * @param [in] str Input string
 * @param [out] end Pointer to first character after the number in input string
 * @param [out] abs_out Absolute part of result
 * @param [out] sign_out Sign part of result (true=negative, false=positive)
 * @param [in] base Integer base
 * @param [in] max_abs_negative Largest permissible absolute part of result if
 * the sign is negative
 * @param [in] max_positive Largest permissible absolute part of result if the
 * sign is positive
 */
StrintParseError strint_to_uint64_internal(
    const char* str,
    char** end,
    uint64_t* abs_out,
    bool* sign_out,
    uint8_t base,
    uint64_t max_abs_negative,
    uint64_t max_positive) {
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
    if(max_abs_negative == 0 && negative) return StrintParseSignError;

    // infer base
    // not assigning directly to `base' to permit prefixes with explicit bases
    uint8_t inferred_base = 0;
    if(memcmp(str, "0x", 2) == 0 || memcmp(str, "0X", 2) == 0) {
        inferred_base = 16;
        str += 2;
    } else if(memcmp(str, "0b", 2) == 0 || memcmp(str, "0B", 2) == 0) {
        inferred_base = 2;
        str += 2;
    } else if(*str == '0') {
        inferred_base = 8;
        str++;
    } else {
        inferred_base = 10;
    }
    if(base == 0) base = inferred_base;

    // read digits
    uint64_t limit = negative ? max_abs_negative : max_positive;
    uint64_t mul_limit = limit / base;
    uint64_t result = 0;
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

        if(result > mul_limit) return StrintParseOverflowError;
        result *= base;
        if(result > limit - digit_value) return StrintParseOverflowError;
        result += digit_value;

        read_total++;
        str++;
    }

    if(read_total == 0) {
        if(inferred_base == 8) {
            // there's just a single zero
            result = 0;
        } else {
            return StrintParseAbsentError;
        }
    }

    if(abs_out) *abs_out = result;
    if(sign_out) *sign_out = negative;
    if(end) *end = (char*)str;
    return StrintParseNoError;
}

#define STRINT_MONO(name, type, neg, pos)                                              \
    StrintParseError name(const char* str, char** end, type* out, uint8_t base) {      \
        uint64_t absolute;                                                             \
        bool sign;                                                                     \
        StrintParseError err =                                                         \
            strint_to_uint64_internal(str, end, &absolute, &sign, base, (neg), (pos)); \
        if(err) return err;                                                            \
        if(out) *out = (sign ? (-(type)absolute) : ((type)absolute));                  \
        return StrintParseNoError;                                                     \
    }

STRINT_MONO(strint_to_uint64, uint64_t, 0, UINT64_MAX)
STRINT_MONO(strint_to_int64, int64_t, (uint64_t)INT64_MAX + 1, INT64_MAX)
STRINT_MONO(strint_to_uint32, uint32_t, 0, UINT32_MAX)
STRINT_MONO(strint_to_int32, int32_t, (uint64_t)INT32_MAX + 1, INT32_MAX)
STRINT_MONO(strint_to_uint16, uint16_t, 0, UINT16_MAX)
STRINT_MONO(strint_to_int16, int16_t, (uint64_t)INT16_MAX + 1, INT16_MAX)
