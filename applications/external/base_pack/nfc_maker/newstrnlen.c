#include "newstrnlen.h"

size_t newstrnlen(const char* s, size_t maxlen) {
    size_t len;

    for(len = 0; len < maxlen; len++, s++) {
        if(!*s) break;
    }

    return len;
}