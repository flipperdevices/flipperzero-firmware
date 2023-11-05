
// Calculates the modulo of a number. Works correctly for negative numbers.
int modulo(int x, int N);

// Wraps a number between min and max (inclusive).
int wrap(int x, int min, int max);

// Wraps a number between min and max (inclusive). When wrapping around, the result will always be min or max.
int wrap_single(int x, int min, int max);

// Linearly interpolates between a and b.
int lerp(int a, int b, float t);

// Calculates the inverse linear interpolation of x between a and b.
float inverse_lerp(int a, int b, int x);

// Maps a number from one range to another.
int map(int a0, int b0, int a1, int b1, int x);