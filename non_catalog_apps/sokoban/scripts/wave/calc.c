#include "calc.h"

int modulo(int x, int N)
{
    return (x % N + N) % N;
}

int wrap(int x, int min, int max)
{
    return modulo(x - min, max - min + 1) + min;
}

int wrap_single(int x, int min, int max)
{
    if (x < min)
        return max;
    if (x > max)
        return min;
    return x;
}

int clamp(int x, int min, int max)
{
    if (x < min)
        return min;
    if (x > max)
        return max;
    return x;
}

int lerp(int a, int b, float t)
{
    return a + (b - a) * t;
}

float inverse_lerp(int a, int b, int x)
{
    return (x - a) / (b - a);
}

int map(int a0, int b0, int a1, int b1, int x)
{
    return lerp(a1, b1, inverse_lerp(a0, b0, x));
}