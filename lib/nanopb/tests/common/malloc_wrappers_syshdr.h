/* This is just a wrapper in order to get our own malloc wrappers into nanopb core. */

#define pb_realloc(ptr,size) realloc_with_check(ptr,size)
#define pb_free(ptr) free_with_check(ptr)

#ifdef PB_OLD_SYSHDR
#include PB_OLD_SYSHDR
#else
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#endif

#include <malloc_wrappers.h>
