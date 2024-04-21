#include <math.h>

#include "clock.h"

#define CLOCK_OFS_X 63
#define CLOCK_OFS_Y 31

#define H_RAD 17
#define M_RAD 26
#define S_RAD 29
#define HMS_OFS 8

#define FACE_RADIUS 31

#define FACE_DEFAULT_WIDTH 54

const char* WEEKDAYS[] =
    {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const char* MONTHS[] =
    {"JAN", "FEB", "MAR", "APR", "MAI", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

void draw_line(Canvas* c, Point* ofs, Line* l, LineType type) { // Bresenham-Algorithm
    int8_t x = l->start.x, y = l->start.y;
    int8_t dx = abs(l->end.x - x), sx = x < l->end.x ? 1 : -1;
    int8_t dy = -abs(l->end.y - y), sy = y < l->end.y ? 1 : -1;
    int8_t error = dx + dy, e2;
    while(true) {
        if(type == Thick)
            canvas_draw_disc(c, ofs->x + x, ofs->y - y, 1);
        else {
            canvas_draw_dot(c, ofs->x + x, ofs->y - y);
            if(type & 1) canvas_draw_dot(c, ofs->x - x, ofs->y - y); // copy hor or both
            if(type & 2) canvas_draw_dot(c, ofs->x + x, ofs->y + y); // copy ver or both
            if(type == CopyBoth) canvas_draw_dot(c, ofs->x - x, ofs->y + y);
        }
        if((x == l->end.x) && (y == l->end.y)) break;
        e2 = 2 * error;
        if(e2 > dy) {
            error += dy;
            x += sx;
        }
        if(e2 < dx) {
            error += dx;
            y += sy;
        }
    }
}

void set_point(Point* p, float ang, double radius) {
    p->x = round(sin(ang) * radius);
    p->y = round(cos(ang) * radius);
}

void intersect(Point* p, float ang, uint8_t width, uint8_t height) { // Quadrant I only
    float t = tan(ang), x = height * t, y = width / t;
    p->x = round(x > width ? width : x);
    p->y = round(y > height ? height : y);
}

void copy_point(Point* p, Point* from, bool flip_x, bool flip_y) {
    p->x = flip_x ? -from->x : from->x;
    p->y = flip_y ? -from->y : from->y;
}

void set_line(Line* l, float ang, float start_rad, float end_rad) {
    set_point(&l->start, ang, start_rad);
    set_point(&l->end, ang, end_rad);
}

void copy_line(Line* l, Line* from, float flip_x, float flip_y) {
    copy_point(&l->start, &from->start, flip_x, flip_y);
    copy_point(&l->end, &from->end, flip_x, flip_y);
}

void draw_hand(Canvas* canvas, ClockConfig* cfg, float ang, int radius, bool thick) {
    Line l;
    set_line(&l, ang, thick ? HMS_OFS : -HMS_OFS, radius);
    draw_line(canvas, &cfg->ofs, &l, thick ? Thick : Normal);
    if(thick) {
        l.end.x = 0;
        l.end.y = 0;
        draw_line(canvas, &cfg->ofs, &l, Normal);
    }
}

void calc_clock_face(ClockConfig* cfg) {
    bool digital = (cfg->face_type == DigitalRectangular) || (cfg->face_type == DigitalRound);
    bool round = ((cfg->face_type == Round) || (cfg->face_type == DigitalRound)) && cfg->split;
    bool dots = digital && cfg->split;

    cfg->ofs.x = cfg->split ? CLOCK_OFS_X / 2 : CLOCK_OFS_X;
    cfg->ofs.y = CLOCK_OFS_Y;

    uint8_t width = cfg->split || round ? FACE_RADIUS : cfg->width;
    uint8_t height = FACE_RADIUS;

    float short_lin_ofs = dots ? 0.0 : round ? 1.5 : 2.0;
    float long_lin_ofs = dots ? 0.0 : round ? 5.0 : 7.0;
    float dig_ofs = round ? 11.0 : 12.5;

    set_line(&cfg->face.minutes[0], 0, height, height - long_lin_ofs);
    set_line(&cfg->face.minutes[15], M_PI_2, width, width - long_lin_ofs);
    copy_line(&cfg->face.minutes[30], &cfg->face.minutes[0], false, true);
    copy_line(&cfg->face.minutes[45], &cfg->face.minutes[15], true, false);

    set_point(&cfg->face.hours[0], 0, height - dig_ofs);
    set_point(&cfg->face.hours[3], M_PI_2, width - dig_ofs);
    copy_point(&cfg->face.hours[6], &cfg->face.hours[0], false, true);
    copy_point(&cfg->face.hours[9], &cfg->face.hours[3], true, false);

    float ang = M_TWOPI / 60;
    for(uint8_t min = 1; min < 15; min++, ang += M_TWOPI / 60) { // 1/4 circle (14 min)
        bool at_hour = (min % 5 == 0);
        float lin_ofs = at_hour ? long_lin_ofs : short_lin_ofs;

        if(round)
            set_line(&cfg->face.minutes[min], ang, FACE_RADIUS, FACE_RADIUS - lin_ofs);
        else {
            intersect(&cfg->face.minutes[min].start, ang, width, height);
            intersect(&cfg->face.minutes[min].end, ang, width - lin_ofs, height - lin_ofs);
        }

        copy_line(&cfg->face.minutes[30 - min], &cfg->face.minutes[min], false, true);
        copy_line(&cfg->face.minutes[30 + min], &cfg->face.minutes[min], true, true);
        copy_line(&cfg->face.minutes[60 - min], &cfg->face.minutes[min], true, false);

        if(at_hour) {
            uint8_t hour = min / 5;
            if(round)
                set_point(&cfg->face.hours[hour], ang, height - dig_ofs);
            else
                intersect(&cfg->face.hours[hour], ang, width - dig_ofs, height - dig_ofs);
            copy_point(&cfg->face.hours[6 - hour], &cfg->face.hours[hour], false, true);
            copy_point(&cfg->face.hours[6 + hour], &cfg->face.hours[hour], true, true);
            copy_point(&cfg->face.hours[12 - hour], &cfg->face.hours[hour], true, false);
        }
    }
}

void draw_digital_clock(Canvas* canvas, ClockConfig* cfg, DateTime* dt) {
    uint8_t hour = dt->hour;
    uint8_t x = cfg->ofs.x;
    uint8_t y = cfg->ofs.y;
    if(locale_get_time_format() == LocaleTimeFormat12h) {
        hour = hour % 12 == 0 ? 12 : hour % 12;
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(
            canvas, x, y - 10, AlignCenter, AlignBottom, (dt->hour >= 12 ? "PM" : "AM"));
    }
    FuriString* time = furi_string_alloc();
    furi_string_printf(time, "%2u:%02u", hour, dt->minute);
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, x, y, AlignCenter, AlignCenter, furi_string_get_cstr(time));
    furi_string_free(time);
    for(int i = 0; i < 45; i++)
        draw_line(canvas, &cfg->ofs, &cfg->face.minutes[(dt->second - i + 60) % 60], Normal);
}

void draw_analog_clock(Canvas* canvas, ClockConfig* cfg, DateTime* dt, int ms) {
    for(int i = 1; i < 15; i++) draw_line(canvas, &cfg->ofs, &cfg->face.minutes[i], CopyBoth);
    draw_line(canvas, &cfg->ofs, &cfg->face.minutes[0], CopyVer);
    draw_line(canvas, &cfg->ofs, &cfg->face.minutes[15], CopyHor);
    if(cfg->digits_mod <= 12) {
        canvas_set_font(canvas, FontSecondary);
        FuriString* num = furi_string_alloc();
        for(int hour = 0; hour < 12; hour++)
            if(hour % cfg->digits_mod == 0) {
                Point* h = &cfg->face.hours[hour];
                uint8_t x = cfg->ofs.x + h->x;
                uint8_t y = cfg->ofs.y - h->y + 1;
                furi_string_printf(num, "%u", hour == 0 ? 12 : hour);
                canvas_draw_str_aligned(
                    canvas, x, y, AlignCenter, AlignCenter, furi_string_get_cstr(num));
            }
        furi_string_free(num);
    }
    float s_ang = M_PI / 30.0 * dt->second + M_PI / 30000.0 * ms;
    float m_ang = M_PI / 30.0 * dt->minute + M_PI / 1800.0 * dt->second;
    float h_ang = M_PI / 6.0 * (dt->hour % 12) + M_PI / 360.0 * dt->minute;
    draw_hand(canvas, cfg, h_ang, H_RAD, true);
    draw_hand(canvas, cfg, m_ang, M_RAD, true);
    draw_hand(canvas, cfg, s_ang, S_RAD, false);
    canvas_draw_disc(canvas, cfg->ofs.x, cfg->ofs.y, 2);
}

void draw_date(Canvas* canvas, ClockConfig* cfg, DateTime* dt) {
    uint8_t x = 96, y = cfg->ofs.y;
    FuriString* dat = furi_string_alloc();
    furi_string_printf(dat, "%2u", dt->day);
    const char* day = furi_string_get_cstr(dat);
    const char* month = MONTHS[(dt->month - 1) % 12];
    const char* weekday = WEEKDAYS[(dt->weekday - 1) % 7];
    int8_t x_ofs = 2;
    Align day_align = AlignLeft, month_align = AlignRight;
    if(locale_get_date_format() == LocaleDateFormatDMY) {
        x_ofs = -2;
        day_align = AlignRight;
        month_align = AlignLeft;
    }
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, x + x_ofs, y, day_align, AlignCenter, day);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, x - x_ofs, y + 3, month_align, AlignCenter, month);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, x, y + 10, AlignCenter, AlignTop, weekday);
    furi_string_free(dat);
}

void draw_clock(Canvas* canvas, ClockConfig* cfg, DateTime* dt, int ms) {
    bool digital = (cfg->face_type == DigitalRectangular) || (cfg->face_type == DigitalRound);
    if(cfg->split && digital)
        draw_digital_clock(canvas, cfg, dt);
    else
        draw_analog_clock(canvas, cfg, dt, ms);
    if(cfg->split) draw_date(canvas, cfg, dt);
}

void init_clock_config(ClockConfig* cfg) {
    cfg->version = CONFIG_VERSION;
    cfg->split = false;
    cfg->width = FACE_DEFAULT_WIDTH;
    cfg->digits_mod = 3;
    cfg->face_type = Rectangular;
}

void modify_clock_up(ClockConfig* cfg) {
    if(cfg->digits_mod < 3)
        cfg->digits_mod += 1;
    else if(cfg->digits_mod <= 12)
        cfg->digits_mod *= 2;
}

void modify_clock_down(ClockConfig* cfg) {
    if(cfg->digits_mod >= 6)
        cfg->digits_mod /= 2;
    else if(cfg->digits_mod > 1)
        cfg->digits_mod -= 1;
}

void modify_clock_left(ClockConfig* cfg) {
    if(cfg->split) {
        cfg->face_type = (cfg->face_type + 1) % FACE_TYPES;
    } else
        cfg->width = cfg->width <= FACE_RADIUS ? FACE_RADIUS : cfg->width - 1;
}

void modify_clock_right(ClockConfig* cfg) {
    if(!cfg->split) cfg->width = cfg->width >= 63 ? 63 : cfg->width + 1;
}

void modify_clock_ok(ClockConfig* cfg) {
    cfg->split = !cfg->split;
}