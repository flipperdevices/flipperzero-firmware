#pragma once
#pragma weak strnlen

#include <stddef.h>

size_t newstrnlen(const char* s, size_t maxlen);