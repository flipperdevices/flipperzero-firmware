#include <math.h>

#include "clock.h"

#define OFS_LEFT_X 31
#define OFS_MID_X 63
#define OFS_RIGHT_X 96
#define OFS_Y 31

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

void draw_line(Canvas* c, uint8_t ofs_x, Line* l, LineType type) { // Bresenham-Algorithm
    int8_t x = l->start.x, y = l->start.y;
    int8_t dx = abs(l->end.x - x), sx = x < l->end.x ? 1 : -1;
    int8_t dy = -abs(l->end.y - y), sy = y < l->end.y ? 1 : -1;
    int8_t error = dx + dy, e2;
    while(true) {
        if(type == Thick)
            canvas_draw_disc(c, ofs_x + x, OFS_Y - y, 1);
        else {
            canvas_draw_dot(c, ofs_x + x, OFS_Y - y);
            if(type & 1) canvas_draw_dot(c, ofs_x - x, OFS_Y - y); // copy hor or both
            if(type & 2) canvas_draw_dot(c, ofs_x + x, OFS_Y + y); // copy ver or both
            if(type == CopyBoth) canvas_draw_dot(c, ofs_x - x, OFS_Y + y);
        }
        if((x == l->end.x) && (y == l->end.y)) break;
        e2 = 2 * error;
        if(e2 > dy) x += sx, error += dy;
        if(e2 < dx) y += sy, error += dx;
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

void draw_hand(Canvas* canvas, uint8_t ofs_x, float ang, int radius, bool thick) {
    Line l;
    set_line(&l, ang, thick ? HMS_OFS : -HMS_OFS, radius);
    draw_line(canvas, ofs_x, &l, thick ? Thick : Normal);
    if(thick) l.end.x = 0, l.end.y = 0, draw_line(canvas, ofs_x, &l, Normal);
}

void calc_clock_face(ClockConfig* cfg) {
    bool digital = (cfg->face_type == DigitalRectangular) || (cfg->face_type == DigitalRound);
    bool round = ((cfg->face_type == Round) || (cfg->face_type == DigitalRound)) && cfg->split;
    bool dots = digital && cfg->split;

    uint8_t width = cfg->split || round ? FACE_RADIUS : cfg->width;
    uint8_t height = FACE_RADIUS;

    float short_ofs = dots ? 1.0 : round ? 1.5 : 2.0;
    float long_ofs = dots ? 1.0 : round ? 5.0 : 7.0;
    float hour_ofs = round ? 11.0 : 12.5;

    set_line(&cfg->face.minutes[0], 0, height, height - long_ofs);
    set_line(&cfg->face.minutes[15], M_PI_2, width, width - long_ofs);
    copy_line(&cfg->face.minutes[30], &cfg->face.minutes[0], false, true);
    copy_line(&cfg->face.minutes[45], &cfg->face.minutes[15], true, false);

    set_point(&cfg->face.hours[0], 0, height - hour_ofs);
    set_point(&cfg->face.hours[3], M_PI_2, width - hour_ofs);
    copy_point(&cfg->face.hours[6], &cfg->face.hours[0], false, true);
    copy_point(&cfg->face.hours[9], &cfg->face.hours[3], true, false);

    float ang = M_TWOPI / 60;
    for(uint8_t min = 1; min < 15; min++, ang += M_TWOPI / 60) { // 1/4 circle (13 min)
        bool at_hour = (min % 5 == 0);
        float ofs = at_hour ? long_ofs : short_ofs;

        if(round)
            set_line(&cfg->face.minutes[min], ang, FACE_RADIUS, FACE_RADIUS - ofs);
        else {
            intersect(&cfg->face.minutes[min].start, ang, width, height);
            intersect(&cfg->face.minutes[min].end, ang, width - ofs, height - ofs);
        }

        copy_line(&cfg->face.minutes[30 - min], &cfg->face.minutes[min], false, true);
        copy_line(&cfg->face.minutes[30 + min], &cfg->face.minutes[min], true, true);
        copy_line(&cfg->face.minutes[60 - min], &cfg->face.minutes[min], true, false);

        if(at_hour) {
            uint8_t hour = min / 5;

            if(round)
                set_point(&cfg->face.hours[hour], ang, height - hour_ofs);
            else
                intersect(&cfg->face.hours[hour], ang, width - hour_ofs, height - hour_ofs);

            copy_point(&cfg->face.hours[6 - hour], &cfg->face.hours[hour], false, true);
            copy_point(&cfg->face.hours[6 + hour], &cfg->face.hours[hour], true, true);
            copy_point(&cfg->face.hours[12 - hour], &cfg->face.hours[hour], true, false);
        }
    }
}

void draw_digital_clock(Canvas* canvas, ClockConfig* cfg, DateTime* dt, uint16_t ms) {
    static char buf[6];
    uint8_t hour = dt->hour;
    if(locale_get_time_format() == LocaleTimeFormat12h) {
        char* pm = hour >= 12 ? "PM" : "AM";
        hour = hour % 12 == 0 ? 12 : hour % 12;
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, cfg->ofs_x, OFS_Y - 10, AlignCenter, AlignBottom, pm);
    }
    snprintf(buf, 6, "%2u:%02u", hour % 24, dt->minute % 60);
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, cfg->ofs_x, OFS_Y, AlignCenter, AlignCenter, buf);
    if(cfg->face_type == DigitalRectangular) {
        for(uint8_t i = 0; i < 45; i++)
            draw_line(canvas, cfg->ofs_x, &cfg->face.minutes[(dt->second - i + 60) % 60], Normal);
    } else {
        Line l;
        for(uint8_t i = 10; i > 0; i--) {
            Line* prev = &cfg->face.minutes[(dt->second - i - 1 + 60) % 60];
            l = cfg->face.minutes[(dt->second - i + 60) % 60];
            if(i > 3)
                canvas_draw_dot(canvas, cfg->ofs_x + l.start.x, OFS_Y - l.start.y);
            else {
                l.end = prev->start;
                draw_line(canvas, cfg->ofs_x, &l, Normal);
            }
        }
        float s_ang = M_TWOPI / 60.0 * dt->second + M_TWOPI / 60000.0 * ms;
        set_point(&l.end, s_ang, FACE_RADIUS - 1);
        draw_line(canvas, cfg->ofs_x, &l, Normal);
        canvas_draw_disc(canvas, cfg->ofs_x + l.end.x, OFS_Y - l.end.y, 1);
    }
}

void draw_analog_clock(Canvas* canvas, ClockConfig* cfg, DateTime* dt, uint16_t ms) {
    static char buf[3];
    Line* m = &cfg->face.minutes[1];
    uint8_t i = 0;
    if(cfg->digits_mod <= 12) {
        canvas_set_font(canvas, FontSecondary);
        Point* h = &cfg->face.hours[0];
        for(; i < 12; i++, m++, h++) {
            if(i % cfg->digits_mod == 0) {
                snprintf(buf, 3, "%2u", i == 0 ? 12 : i);
                canvas_draw_str_aligned(
                    canvas, cfg->ofs_x + h->x, OFS_Y - h->y + 1, AlignCenter, AlignCenter, buf);
            }
            draw_line(canvas, cfg->ofs_x, m, CopyBoth);
        }
    }
    for(; i < 14; i++, m++) draw_line(canvas, cfg->ofs_x, m, CopyBoth);
    draw_line(canvas, cfg->ofs_x, &cfg->face.minutes[0], CopyVer);
    draw_line(canvas, cfg->ofs_x, &cfg->face.minutes[15], CopyHor);
    float s_ang = M_TWOPI / 60.0 * dt->second + M_TWOPI / 60000.0 * ms;
    float m_ang = M_TWOPI / 60.0 * dt->minute + M_TWOPI / 3600.0 * dt->second;
    float h_ang = M_TWOPI / 12.0 * (dt->hour % 12) + M_TWOPI / 720.0 * dt->minute;
    draw_hand(canvas, cfg->ofs_x, h_ang, H_RAD, true);
    draw_hand(canvas, cfg->ofs_x, m_ang, M_RAD, true);
    draw_hand(canvas, cfg->ofs_x, s_ang, S_RAD, false);
    canvas_draw_disc(canvas, cfg->ofs_x, OFS_Y, 2);
}

void draw_date(Canvas* canvas, DateTime* dt) {
    static char day[3];
    snprintf(day, 3, "%2u", dt->day % 32);
    const char* month = MONTHS[(dt->month - 1) % 12];
    const char* weekday = WEEKDAYS[(dt->weekday - 1) % 7];
    int8_t ofs_x = 2;
    Align d_align = AlignLeft;
    Align m_align = AlignRight;
    if(locale_get_date_format() == LocaleDateFormatDMY)
        ofs_x = -2, d_align = AlignRight, m_align = AlignLeft;
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, OFS_RIGHT_X + ofs_x, OFS_Y, d_align, AlignCenter, day);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, OFS_RIGHT_X - ofs_x, OFS_Y + 3, m_align, AlignCenter, month);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, OFS_RIGHT_X, OFS_Y + 10, AlignCenter, AlignTop, weekday);
}

void draw_clock(Canvas* canvas, ClockConfig* cfg, DateTime* dt, uint16_t ms) {
    bool digital = (cfg->face_type == DigitalRectangular) || (cfg->face_type == DigitalRound);
    if(cfg->split && digital)
        draw_digital_clock(canvas, cfg, dt, ms);
    else
        draw_analog_clock(canvas, cfg, dt, ms);
    if(cfg->split) draw_date(canvas, dt);
}

void init_clock_config(ClockConfig* cfg) {
    cfg->version = CONFIG_VERSION;
    cfg->split = false;
    cfg->width = FACE_DEFAULT_WIDTH;
    cfg->digits_mod = 3;
    cfg->face_type = Rectangular;
    cfg->ofs_x = OFS_MID_X;
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
    if(cfg->split)
        cfg->face_type = (cfg->face_type + 1) % FACE_TYPES;
    else
        cfg->width = cfg->width <= FACE_RADIUS ? FACE_RADIUS : cfg->width - 1;
}

void modify_clock_right(ClockConfig* cfg) {
    if(!cfg->split) cfg->width = cfg->width >= OFS_MID_X ? OFS_MID_X : cfg->width + 1;
}

void modify_clock_ok(ClockConfig* cfg) {
    cfg->split = !cfg->split;
    cfg->ofs_x = cfg->split ? OFS_LEFT_X : OFS_MID_X;
}
