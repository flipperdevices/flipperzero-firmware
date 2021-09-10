/* This wrapper undefines (u)int64_t */

#ifdef PB_OLD_SYSHDR
#include PB_OLD_SYSHDR
#else
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#endif

#define uint64_t disabled_uint64_t
#define int64_t disabled_int64_t

