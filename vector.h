#pragma once

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

#ifdef __cplusplus
}
#endif