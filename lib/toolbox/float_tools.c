#include "float_tools.h"

#include <math.h>
#include <float.h>

bool float_is_equal(float a, float b) {
    return fabsf(fabsf(a) - fabsf(b)) <= FLT_EPSILON * fmaxf(fabsf(a), fabsf(b));
}
