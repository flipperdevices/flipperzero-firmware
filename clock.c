#include <math.h>

#include "clock.h"

#define CLOCK_OFS_X 63.5
#define CLOCK_OFS_Y 31.5

#define M_RAD 28
#define H_RAD 17
#define HM_OFS 8
#define HM_WIDTH 5
#define S_RAD 29
#define S_EXT 7

#define FACE_RADIUS 31.0
#define FACE_HEIGHT 31.0

#define FACE_DEFAULT_WIDTH 54

#define LO_TRESHOLD (1.0 / 32.0)
#define HI_TRESHOLD (1.0 - LO_TRESHOLD)

float wide_line_distance(float xpax, float ypay, float bax, float bay) {
    float h = fmaxf(fminf((xpax * bax + ypay * bay) / (bax * bax + bay * bay), 1.0f), 0.0f);
    float dx = xpax - bax * h, dy = ypay - bay * h;
    return sqrtf(dx * dx + dy * dy);
}

void draw_wide_line(Canvas* c, float ax, float ay, float bx, float by, float wd) {
    if((fabsf(ax - bx) < 0.01f) && (fabsf(ay - by) < 0.01f)) bx += 0.01f; // Avoid divide by zero

    // Find line bounding box
    float r = wd / 2;
    int x0 = (int)floorf(fminf(ax - r, bx - r));
    int x1 = (int)ceilf(fmaxf(ax + r, bx + r));
    int y0 = (int)floorf(fminf(ay - r, by - r));
    int y1 = (int)ceilf(fmaxf(ay + r, by + r));

    // Establish x start and y start
    int ys = ay;
    if((ax - r) > (bx - r)) ys = by;

    float alpha = 1.0f;
    r += 0.5;

    float xpax, ypay, bax = bx - ax, bay = by - ay;

    int xs = x0;
    // Scan bounding box from ys down, calculate pixel intensity from distance to line
    for(int yp = ys; yp <= y1; yp++) {
        bool endX = false; // Flag to skip pixels
        ypay = yp - ay;
        for(int xp = xs; xp <= x1; xp++) {
            if(endX && (alpha <= LO_TRESHOLD)) break; // Skip right side
            xpax = xp - ax;
            alpha = r - wide_line_distance(xpax, ypay, bax, bay);
            if(alpha <= LO_TRESHOLD) continue;
            // Track edge to minimise calculations
            if(!endX) {
                endX = true;
                xs = xp;
            }
            if(alpha > HI_TRESHOLD) canvas_draw_dot(c, xp, yp);
        }
    }

    // Reset x start to left side of box
    xs = x0;
    // Scan bounding box from ys-1 up, calculate pixel intensity from distance to line
    for(int yp = ys - 1; yp >= y0; yp--) {
        bool endX = false; // Flag to skip pixels
        ypay = yp - ay;
        for(int xp = xs; xp <= x1; xp++) {
            if(endX && (alpha <= LO_TRESHOLD)) break; // Skip right side of drawn line
            xpax = xp - ax;
            alpha = r - wide_line_distance(xpax, ypay, bax, bay);
            if(alpha <= LO_TRESHOLD) continue;
            // Track line boundary
            if(!endX) {
                endX = true;
                xs = xp;
            }
            if(alpha > HI_TRESHOLD) canvas_draw_dot(c, xp, yp);
        }
    }
}

char* clock_number_str(uint8_t number) {
    // itoa(number % 100, str, 10);
    static char str[3];

    if(number == 0) number = 12;
    str[0] = '0' + (number / 10) % 10;
    str[1] = '0' + number % 10;
    str[2] = 0;
    if(str[0] == '0') {
        str[0] = str[1];
        str[1] = 0;
    }
    return str;
}

void draw_hand(Canvas* canvas, float x, float y, float ang, float width, int radius, int ofs) {
    float s = sin(ang);
    float c = -cos(ang);
    canvas_draw_line(canvas, x, y, round(s * ofs + x), round(c * ofs + y));
    draw_wide_line(canvas, s * ofs + x, c * ofs + y, s * radius + x, c * radius + y, width);
}

void draw_sec_hand(Canvas* canvas, float x, float y, float ang, float radius, float ext) {
    float s = sin(ang);
    float c = -cos(ang);
    canvas_draw_line(
        canvas,
        round(s * -ext + x),
        round(c * -ext + y),
        round(s * radius + x),
        round(c * radius + y));
}

float intersect_x(float tan, float width, float height) {
    float x = height / tan;
    return x > width ? width : x;
}

float intersect_y(float tan, float width, float height) {
    float y = width * tan;
    return y > height ? height : y;
}

inline void set_clock_minute_line(
    ClockConfig* cfg,
    uint8_t idx,
    float start_x,
    float start_y,
    float end_x,
    float end_y) {
    furi_assert(cfg);
    furi_assert(idx < 60);
    round(cfg->face.minute_lines[idx].start.x = start_x + cfg->face.ctr.x);
    round(cfg->face.minute_lines[idx].start.y = start_y + cfg->face.ctr.y);
    round(cfg->face.minute_lines[idx].end.x = end_x + cfg->face.ctr.x);
    round(cfg->face.minute_lines[idx].end.y = end_y + cfg->face.ctr.y);
}

inline void set_clock_hour_point(ClockConfig* cfg, uint8_t idx, float x, float y) {
    furi_assert(cfg);
    furi_assert(idx < 12);
    round(cfg->face.hour_points[idx].x = x + cfg->face.ctr.x);
    round(cfg->face.hour_points[idx].y = y + cfg->face.ctr.y + 1);
}

void calc_clock_face(ClockConfig* cfg) {
    furi_assert(cfg);

    bool digital = (cfg->face.type == DigitalRectangular) || (cfg->face.type == DigitalRound);
    bool roundFace = (cfg->face.type == Round) || (cfg->face.type == DigitalRound);
    bool round = cfg->split ? roundFace : false;
    bool dots = digital && cfg->split;

    float width = cfg->split ? FACE_HEIGHT : cfg->width;
    float horOfs = round ? FACE_RADIUS : FACE_HEIGHT;
    float verOfs = round ? FACE_RADIUS : width;

    cfg->face.ctr.x = cfg->split ? CLOCK_OFS_X / 2.0 : CLOCK_OFS_X;
    cfg->face.ctr.y = CLOCK_OFS_Y;

    float shortLineLen = dots ? 0.0 : round ? 1.0 : 2.0;
    float longLineLen = dots ? 0.0 : round ? 3.5 : 6.5;
    float digitsOfs = round ? 10.0 : 13.0;

    set_clock_minute_line(cfg, 0, 0, -horOfs, 0, longLineLen - horOfs);
    set_clock_minute_line(cfg, 15, verOfs, 0, verOfs - longLineLen, 0);
    set_clock_minute_line(cfg, 30, 0, horOfs, 0, horOfs - longLineLen);
    set_clock_minute_line(cfg, 45, -verOfs, 0, longLineLen - verOfs, 0);

    set_clock_hour_point(cfg, 0, 0, digitsOfs - horOfs);
    set_clock_hour_point(cfg, 3, verOfs - digitsOfs, 0);
    set_clock_hour_point(cfg, 6, 0, horOfs - digitsOfs);
    set_clock_hour_point(cfg, 9, digitsOfs - verOfs, 0);

    float inc = M_PI / 30;
    float ang = inc;
    for(int i = 1; i < 15; i++) { // 1/4 circle
        bool atHour = (i % 5 == 0);

        float lineLen = atHour ? longLineLen : shortLineLen;

        float startPtX, startPtY;
        float endPtX, endPtY;
        float digitPosX, digitPosY;

        if(round) {
            float c = cos(ang);
            float s = sin(ang);
            startPtX = c * FACE_RADIUS;
            startPtY = s * FACE_RADIUS;
            endPtX = c * (FACE_RADIUS - lineLen);
            endPtY = s * (FACE_RADIUS - lineLen);
            digitPosX = c * (FACE_RADIUS - digitsOfs);
            digitPosY = s * (FACE_RADIUS - digitsOfs);
        } else {
            float t = tan(ang);
            startPtX = intersect_x(t, width, FACE_HEIGHT);
            startPtY = intersect_y(t, width, FACE_HEIGHT);
            endPtX = intersect_x(t, width - lineLen, FACE_HEIGHT - lineLen);
            endPtY = intersect_y(t, width - lineLen, FACE_HEIGHT - lineLen);
            digitPosX = intersect_x(t, width - digitsOfs, FACE_HEIGHT - digitsOfs);
            digitPosY = intersect_y(t, width - digitsOfs, FACE_HEIGHT - digitsOfs);
        }

        set_clock_minute_line(cfg, 15 - i, startPtX, -startPtY, endPtX, -endPtY);
        set_clock_minute_line(cfg, i + 15, startPtX, startPtY, endPtX, endPtY);
        set_clock_minute_line(cfg, 45 - i, -startPtX, startPtY, -endPtX, endPtY);
        set_clock_minute_line(cfg, i + 45, -startPtX, -startPtY, -endPtX, -endPtY);

        if(atHour) {
            int hour = i / 5;
            set_clock_hour_point(cfg, 3 - hour, digitPosX, -digitPosY);
            set_clock_hour_point(cfg, hour + 3, digitPosX, digitPosY);
            set_clock_hour_point(cfg, 9 - hour, -digitPosX, digitPosY);
            set_clock_hour_point(cfg, hour + 9, -digitPosX, -digitPosY);
        }
        ang += inc; // minute
    }
}

void draw_digital_clock(Canvas* canvas, ClockConfig* cfg, int h, int m, int s) {
    FuriString* time = furi_string_alloc();
    furi_string_printf(time, "%2u:%02u", h, m);
    canvas_set_font(canvas, FontBigNumbers);
    const char* buf = furi_string_get_cstr(time);
    canvas_draw_str_aligned(
        canvas, cfg->face.ctr.x, cfg->face.ctr.y, AlignCenter, AlignCenter, buf);
    furi_string_free(time);

    for(int i = 0; i <= 45; i++) {
        int idx = (s - i + 60) % 60;
        canvas_draw_line(
            canvas,
            cfg->face.minute_lines[idx].start.x,
            cfg->face.minute_lines[idx].start.y,
            cfg->face.minute_lines[idx].end.x,
            cfg->face.minute_lines[idx].end.y);
    }
}

void draw_analog_clock(Canvas* canvas, ClockConfig* cfg, int h, int m, int s, int ms) {
    float secAng = M_PI / 30.0 * s + M_PI / 30.0 / 1000.0 * ms; // min: 2*PI/60 hour: 2*PI/12
    float minAng =
        M_PI / 30.0 * m + M_PI / 30.0 / 60.0 * s + M_PI / 30.0 / 60.0 / 60.0 / 1000.0 * ms;
    float hourAng = M_PI / 6.0 * (h % 12) + M_PI / 6.0 / 60.0 * m + M_PI / 6.0 / 60.0 / 60.0 * s;

    canvas_set_font(canvas, FontSecondary);
    for(int i = 0; i < 60; i++) {
        if(i % 5 == 0) {
            canvas_draw_str_aligned(
                canvas,
                cfg->face.hour_points[i / 5].x,
                cfg->face.hour_points[i / 5].y,
                AlignCenter,
                AlignCenter,
                clock_number_str(i / 5));
        }
        canvas_draw_line(
            canvas,
            cfg->face.minute_lines[i].start.x,
            cfg->face.minute_lines[i].start.y,
            cfg->face.minute_lines[i].end.x,
            cfg->face.minute_lines[i].end.y);
    }

    draw_hand(canvas, cfg->face.ctr.x, cfg->face.ctr.y, hourAng, HM_WIDTH, H_RAD, HM_OFS);
    draw_hand(canvas, cfg->face.ctr.x, cfg->face.ctr.y, minAng, HM_WIDTH, M_RAD, HM_OFS);
    draw_sec_hand(canvas, cfg->face.ctr.x, cfg->face.ctr.y, secAng, S_RAD, S_EXT);
    canvas_draw_disc(canvas, cfg->face.ctr.x, cfg->face.ctr.y, 2);
}

void draw_clock(Canvas* canvas, ClockConfig* cfg, int h, int m, int s, int ms) {
    bool digital = (cfg->face.type == DigitalRectangular) || (cfg->face.type == DigitalRound);
    if(cfg->split && digital)
        draw_digital_clock(canvas, cfg, h, m, s);
    else
        draw_analog_clock(canvas, cfg, h, m, s, ms);
}

void init_clock_config(ClockConfig* cfg) {
    cfg->split = false;
    cfg->face.type = Rectangular;
    cfg->width = FACE_DEFAULT_WIDTH;
}

void modify_clock_left(ClockConfig* cfg) {
    if(cfg->split) {
        cfg->face.type = (cfg->face.type + 1) % FACE_TYPES;
    } else
        cfg->width = cfg->width <= 32 ? 32 : cfg->width - 1;
}

void modify_clock_right(ClockConfig* cfg) {
    if(!cfg->split) cfg->width = cfg->width >= 63 ? 63 : cfg->width + 1;
}

void modify_clock_ok(ClockConfig* cfg) {
    cfg->split = !cfg->split;
}