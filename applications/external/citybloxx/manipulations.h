#pragma once
#include "types.h"

void point_translate(Point* point, int32_t x, int32_t y);
void point_rotate(Point* point, Point around, float deg);

void line_translate(Line* line, int32_t x, int32_t y);
void line_rotate(Line* line, Point around, float deg);

void line_group_translate(Line* line_group, uint8_t count, int32_t x, int32_t y);
void line_group_rotate(Line* line_group, uint8_t count, Point around, float deg);
Point line_group_get_center(Line* line_group, uint8_t count);
Point line_group_rotate_center(Line* line_group, uint8_t count, float deg);
