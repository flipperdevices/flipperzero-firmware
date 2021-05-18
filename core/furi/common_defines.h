#pragma once

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef CLAMP
#define CLAMP(x, upper, lower) (MIN(upper, MAX(x, lower)))
#endif

// need some common semantics for those two
#ifndef SIZEOF_ARRAY
#define SIZEOF_ARRAY(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

#ifndef COUNT_OF
#define COUNT_OF(x) (sizeof(x) / sizeof(x[0]))
#endif
