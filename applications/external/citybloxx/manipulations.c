#include "manipulations.h"

#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif
#include <furi.h>

#define DEG_TO_RAD(deg) ((deg)*M_PI / 180.0)

#define MIN3(a, b, c) MIN(MIN((a), (b)), (c))
#define MAX3(a, b, c) MAX(MAX((a), (b)), (c))

void point_translate(Point* point, int32_t x, int32_t y) {
    point->x += x;
    point->y += y;
}
void point_rotate(Point* point, Point around, float deg) {
    float rad = DEG_TO_RAD(deg);
    float s = sin(rad);
    float c = cos(rad);
    int32_t dx = point->x - around.x;
    int32_t dy = point->y - around.y;

    point->x = c * dx - s * dy;
    point->y = s * dx + c * dy;

    point->x += around.x;
    point->y += around.y;
}

void line_translate(Line* line, int32_t x, int32_t y) {
    point_translate(&line->a, x, y);
    point_translate(&line->b, x, y);
}
void line_rotate(Line* line, Point around, float deg) {
    point_rotate(&line->a, around, deg);
    point_rotate(&line->b, around, deg);
}

void line_group_translate(Line* line_group, uint8_t count, int32_t x, int32_t y) {
    for(uint8_t i = 0; i < count; i++) line_translate(&line_group[i], x, y);
}
void line_group_rotate(Line* line_group, uint8_t count, Point around, float deg) {
    for(uint8_t i = 0; i < count; i++) line_rotate(&line_group[i], around, deg);
}
Point line_group_get_center(Line* line_group, uint8_t count) {
    // Finding the corners
    int32_t min_x = INT32_MAX, min_y = INT32_MAX, max_x = INT32_MIN, max_y = INT32_MIN;
    for(uint8_t i = 0; i < count; i++) {
        min_x = MIN3(min_x, line_group[i].a.x, line_group[i].b.x);
        min_y = MIN3(min_y, line_group[i].a.y, line_group[i].b.y);
        max_x = MAX3(max_x, line_group[i].a.x, line_group[i].b.x);
        max_y = MAX3(max_y, line_group[i].a.y, line_group[i].b.y);
    }
    // Finding the center
    int32_t center_x = (min_x + max_x) / 2;
    int32_t center_y = (min_y + max_y) / 2;
    const Point center = {center_x, center_y};

    return center;
}
Point line_group_rotate_center(Line* line_group, uint8_t count, float deg) {
    const Point center = line_group_get_center(line_group, count);

    // Rotating around the center
    line_group_rotate(line_group, count, center, deg);
    return center;
}
