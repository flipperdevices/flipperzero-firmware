#include "vector.h"
#include <math.h>
#include <float.h>

#undef vector_add
#undef vector_sub
#undef vector_mul
#undef vector_div

Vector vector_add(Vector a, Vector b) {
    return (Vector){.x = a.x + b.x, .y = a.y + b.y};
}

Vector vector_sub(Vector a, Vector b) {
    return (Vector){.x = a.x - b.x, .y = a.y - b.y};
}

Vector vector_mul(Vector a, Vector b) {
    return (Vector){.x = a.x * b.x, .y = a.y * b.y};
}

Vector vector_div(Vector a, Vector b) {
    return (Vector){.x = a.x / b.x, .y = a.y / b.y};
}

Vector vector_addf(Vector a, float b) {
    return (Vector){.x = a.x + b, .y = a.y + b};
}

Vector vector_subf(Vector a, float b) {
    return (Vector){.x = a.x - b, .y = a.y - b};
}

Vector vector_mulf(Vector a, float b) {
    return (Vector){.x = a.x * b, .y = a.y * b};
}

Vector vector_divf(Vector a, float b) {
    return (Vector){.x = a.x / b, .y = a.y / b};
}

float vector_length(Vector v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

Vector vector_normalize(Vector v) {
    float length = vector_length(v);
    if(length < FLT_EPSILON) {
        return (Vector){0, 0};
    }
    return (Vector){v.x / length, v.y / length};
}

float vector_dot(Vector a, Vector b) {
    return a.x * b.x + a.y * b.y;
}

Vector vector_rand() {
    float x = (rand() % __INT_MAX__) / (float)__INT_MAX__;
    float y = (rand() % __INT_MAX__) / (float)__INT_MAX__;
    return (Vector){x, y};
}