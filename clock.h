#pragma once
#include <furi.h>
#include <gui/gui.h>
#include <locale/locale.h>

#define FACE_TYPES 4
typedef enum {
    Rectangular = 0,
    Round,
    DigitalRectangular,
    DigitalRound,
} FaceType;

typedef struct {
    uint8_t x;
    uint8_t y;
} Point;

typedef struct {
    Point start;
    Point end;
} Line;

typedef struct {
    LocaleTimeFormat time_fmt;
    Point ctr;
    Line minute_lines[60];
    Point hour_points[12];
} ClockFace;

typedef struct {
    bool split;
    FaceType face_type;
    uint8_t digits_mod;
    uint8_t width;
    ClockFace face;
} ClockConfig;

char* clock_number_str(uint8_t number);
void set_clock_hour_point(ClockConfig* cfg, uint8_t idx, float x, float y);
void set_clock_minute_line(
    ClockConfig* cfg,
    uint8_t idx,
    float start_x,
    float start_y,
    float end_x,
    float end_y);
void calc_clock_face(ClockConfig* cfg);

void draw_digital_clock(Canvas* canvas, ClockConfig* cfg, DateTime* dt);
void draw_analog_clock(Canvas* canvas, ClockConfig* cfg, DateTime* dt, int ms);
void draw_date(Canvas* canvas, ClockConfig* cfg, DateTime* dt);

void draw_clock(Canvas* canvas, ClockConfig* cfg, DateTime* dt, int ms);

void init_clock_config(ClockConfig* cfg);
void modify_clock_up(ClockConfig* cfg);
void modify_clock_down(ClockConfig* cfg);
void modify_clock_left(ClockConfig* cfg);
void modify_clock_right(ClockConfig* cfg);
void modify_clock_ok(ClockConfig* cfg);
