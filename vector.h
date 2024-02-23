#pragma once
#include <m-core.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x;
    float y;
} Vector;

#define VECTOR_ZERO ((Vector){0, 0})

Vector vector_add(Vector a, Vector b);

Vector vector_sub(Vector a, Vector b);

Vector vector_mul(Vector a, Vector b);

Vector vector_div(Vector a, Vector b);

Vector vector_addf(Vector a, float b);

Vector vector_subf(Vector a, float b);

Vector vector_mulf(Vector a, float b);

Vector vector_divf(Vector a, float b);

float vector_length(Vector v);

Vector vector_normalize(Vector v);

float vector_dot(Vector a, Vector b);

Vector vector_rand();

#define VECTOR_SELECT(func1, func2, a, b) \
    _Generic(                             \
        (b),                              \
        float: func2,                     \
        const float: func2,               \
        int: func2,                       \
        const int: func2,                 \
        Vector: func1,                    \
        const Vector: func1)(a, b)

#define vector_add(a, b) VECTOR_SELECT(vector_add, vector_addf, a, b)
#define vector_sub(a, b) VECTOR_SELECT(vector_sub, vector_subf, a, b)
#define vector_mul(a, b) VECTOR_SELECT(vector_mul, vector_mulf, a, b)
#define vector_div(a, b) VECTOR_SELECT(vector_div, vector_divf, a, b)

#ifdef __cplusplus
}
#endif