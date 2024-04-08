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

char* WEEKDAYS[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
char* MONTHS[] =
    {"Jan", "Feb", "Mar", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

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

    bool digital = (cfg->face_type == DigitalRectangular) || (cfg->face_type == DigitalRound);
    bool roundFace = (cfg->face_type == Round) || (cfg->face_type == DigitalRound);
    bool round = cfg->split ? roundFace : false;
    bool dots = digital && cfg->split;

    float width = cfg->split ? FACE_HEIGHT : cfg->width;
    float horOfs = round ? FACE_RADIUS : FACE_HEIGHT;
    float verOfs = round ? FACE_RADIUS : width;

    cfg->face.time_fmt = locale_get_time_format();
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

void draw_digital_clock(Canvas* canvas, ClockConfig* cfg, DateTime* dt) {
    uint8_t hour = dt->hour;
    if(cfg->face.time_fmt == LocaleTimeFormat12h) {
        hour = hour % 12 == 0 ? 12 : hour % 12;
        uint8_t x = cfg->face.ctr.x;
        uint8_t y = cfg->face.ctr.y;
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(
            canvas, x, y - 10, AlignCenter, AlignBottom, (dt->hour >= 12 ? "PM" : "AM"));
    }

    FuriString* time = furi_string_alloc();
    furi_string_printf(time, "%2u:%02u", hour, dt->minute);
    canvas_set_font(canvas, FontBigNumbers);
    const char* buf = furi_string_get_cstr(time);
    canvas_draw_str_aligned(
        canvas, cfg->face.ctr.x, cfg->face.ctr.y, AlignCenter, AlignCenter, buf);
    furi_string_free(time);

    for(int i = 0; i <= 45; i++) {
        int idx = (dt->second - i + 60) % 60;
        canvas_draw_line(
            canvas,
            cfg->face.minute_lines[idx].start.x,
            cfg->face.minute_lines[idx].start.y,
            cfg->face.minute_lines[idx].end.x,
            cfg->face.minute_lines[idx].end.y);
    }
}

void draw_analog_clock(Canvas* canvas, ClockConfig* cfg, DateTime* dt, int ms) {
    float secAng = M_PI / 30.0 * dt->second + M_PI / 30000.0 * ms;
    float minAng = M_PI / 30.0 * dt->minute + M_PI / 1800.0 * dt->second;
    float hourAng = M_PI / 6.0 * (dt->hour % 12) + M_PI / 360.0 * dt->minute;

    canvas_set_font(canvas, FontSecondary);
    for(int i = 0; i < 60; i++) {
        if(i % 5 == 0) {
            int hour = i / 5;
            if((cfg->digits_mod <= 12) && (hour % cfg->digits_mod == 0))
                canvas_draw_str_aligned(
                    canvas,
                    cfg->face.hour_points[hour].x,
                    cfg->face.hour_points[hour].y,
                    AlignCenter,
                    AlignCenter,
                    clock_number_str(hour));
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

void draw_date(Canvas* canvas, ClockConfig* cfg, DateTime* dt) {
    uint8_t x = 96;
    uint8_t y = cfg->face.ctr.y;
    FuriString* dat = furi_string_alloc();
    furi_string_printf(dat, "%2u", dt->day);
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, x - 2, y, AlignRight, AlignCenter, furi_string_get_cstr(dat));
    furi_string_free(dat);

    const char* month = MONTHS[(dt->month - 1) % 12];
    const char* weekday = WEEKDAYS[(dt->weekday - 1) % 7];
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, x + 2, y + 3, AlignLeft, AlignCenter, month);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, x, y + 10, AlignCenter, AlignTop, weekday);
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
    cfg->split = false;
    cfg->face_type = Rectangular;
    cfg->digits_mod = 1;
    cfg->width = FACE_DEFAULT_WIDTH;
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
        cfg->width = cfg->width <= 32 ? 32 : cfg->width - 1;
}

void modify_clock_right(ClockConfig* cfg) {
    if(!cfg->split) cfg->width = cfg->width >= 63 ? 63 : cfg->width + 1;
}

void modify_clock_ok(ClockConfig* cfg) {
    cfg->split = !cfg->split;
}