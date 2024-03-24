#ifndef COMMON_H
#define COMMON_H

#include <inttypes.h>

inline uint64_t napi_nfc_util_bytes2num(const uint8_t* src, uint8_t len);

inline uint64_t napi_nfc_util_bytes2num(const uint8_t* src, uint8_t len) {
    furi_assert(src);
    furi_assert(len <= 8);

    uint64_t res = 0;
    while(len--) {
        res = (res << 8) | (*src);
        src++;
    }
    return res;
}

#endif // COMMON_H