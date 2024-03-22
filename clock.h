#pragma once
#include <furi.h>
#include <gui/gui.h>

typedef struct {
    uint8_t x;
    uint8_t y;
} Point;

typedef struct {
    Point start;
    Point end;
} Line;

typedef struct {
    Line minute_lines[60];
    Point hour_points[12];
} ClockFace;

typedef struct {
    bool round_face;
    uint8_t width;
    ClockFace face;
} ClockConfig;

char* clock_number_str(uint8_t number);
void set_clock_hour_point(ClockFace* face, uint8_t idx, float x, float y);
void set_clock_minute_line(
    ClockFace* face,
    uint8_t idx,
    float start_x,
    float start_y,
    float end_x,
    float end_y);
void calc_clock_face(ClockConfig* cfg);

void draw_clock(Canvas* canvas, ClockConfig* cfg, int h, int m, int s, int ms);
