#include "vector.h"

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