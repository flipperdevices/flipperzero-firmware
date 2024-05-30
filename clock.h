#pragma once
#include <furi.h>
#include <gui/gui.h>
#include <locale/locale.h>

#define FACE_TYPES 4
typedef enum {
    Rectangular = 0,
    DigitalRectangular,
    Round,
    DigitalRound,
} FaceType;

typedef enum { Normal = 0, CopyHor, CopyVer, CopyBoth, Thick } LineType;

typedef struct {
    int8_t x;
    int8_t y;
} Point;

typedef struct {
    Point start;
    Point end;
} Line;

typedef struct {
    Line minutes[60];
    Point hours[12];
} ClockFace;

#define CONFIG_VERSION 2
typedef struct {
    uint8_t version;
    bool split;
    uint8_t width;
    uint8_t digits_mod;
    FaceType face_type;
    uint8_t ofs_x;
    ClockFace face;
} ClockConfig;

void calc_clock_face(ClockConfig* cfg);
void draw_clock(Canvas* canvas, ClockConfig* cfg, DateTime* dt, uint16_t ms);

void init_clock_config(ClockConfig* cfg);
void modify_clock_up(ClockConfig* cfg);
void modify_clock_down(ClockConfig* cfg);
void modify_clock_left(ClockConfig* cfg);
void modify_clock_right(ClockConfig* cfg);
void modify_clock_ok(ClockConfig* cfg);
