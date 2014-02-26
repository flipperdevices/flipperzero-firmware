/*
 * Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 */

#include "minmea.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#define boolstr(s) ((s) ? "true" : "false")

static int hex2int(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return -1;
}

bool minmea_check(const char *sentence)
{
    uint8_t checksum = 0x00;

    // Sequence length is limited.
    if (strlen(sentence) > MINMEA_MAX_LENGTH + 3)
        return false;

    // A valid sentence starts with "$".
    if (*sentence++ != '$')
        return false;

    // The optional checksum is an XOR of all bytes between "$" and "*".
    while (*sentence && *sentence != '*' && isprint((unsigned char) *sentence))
        checksum ^= *sentence++;

    if (*sentence == '*') {
        // Extract checksum.
        sentence++;
        int upper = hex2int(*sentence++);
        if (upper == -1)
            return false;
        int lower = hex2int(*sentence++);
        if (lower == -1)
            return false;
        int expected = upper << 4 | lower;

        // Check for checksum mismatch.
        if (checksum != expected)
            return false;
    }

    // The only stuff allowed at this point is a newline.
    if (*sentence && strcmp(sentence, "\n") && strcmp(sentence, "\r\n"))
        return false;

    return true;
}

static inline bool minmea_isfield(char c) {
    return isprint((unsigned char) c) && c != ',' && c != '*';
}

bool minmea_scan(const char *sentence, const char *format, ...)
{
    bool result = false;
    va_list ap;
    va_start(ap, format);

    const char *field = sentence;
#define next_field() \
    do { \
        while (minmea_isfield(*sentence++)) {} \
        field = sentence; \
    } while (0)

    while (*format) {
        char type = *format++;

        switch (type) {
            case 'c': { // Single character field (char).
                char value = '\0';

                if (minmea_isfield(*field))
                    value = *field;
                else
                    value = '\0';

                *va_arg(ap, char *) = value;
            } break;

            case 'd': { // Single character direction field (int).
                int value = 0;

                if (minmea_isfield(*field)) {
                    switch (*field) {
                        case 'N':
                        case 'E':
                            value = 1;
                            break;
                        case 'S':
                        case 'W':
                            value = -1;
                            break;
                        default:
                            goto end;
                    }
                }

                *va_arg(ap, int *) = value;
            } break;

            case 'f': { // Fractional value with scale (int, int).
                int sign = 0;
                int value = -1;
                int scale = 0;

                while (minmea_isfield(*field)) {
                    if (*field == '+' && !sign && value == -1) {
                        sign = 1;
                    } else if (*field == '-' && !sign && value == -1) {
                        sign = -1;
                    } else if (isdigit((unsigned char) *field)) {
                        if (value == -1)
                            value = 0;
                        value = (10 * value) + (*field - '0');
                        if (scale)
                            scale *= 10;
                    } else if (*field == '.' && scale == 0) {
                        scale = 1;
                    } else {
                        goto end;
                    }
                    field++;
                }

                if ((sign || scale) && value == -1)
                    goto end;

                if (value == -1) {
                    value = 0;
                    scale = 0;
                }
                if (sign)
                    value *= sign;

                *va_arg(ap, int *) = value;
                *va_arg(ap, int *) = scale;
            } break;

            case 'i': { // Integer value, default 0 (int).
                int value;

                char *endptr;
                value = strtol(field, &endptr, 10);
                if (minmea_isfield(*endptr))
                    goto end;

                *va_arg(ap, int *) = value;
            } break;

            case 's': { // String value (char *).
                char *buf = va_arg(ap, char *);

                while (minmea_isfield(*field))
                    *buf++ = *field++;
                *buf = '\0';
            } break;

            case 't': { // NMEA talker+sequence identifier (char *).
                if (field[0] != '$')
                    goto end;
                for (int i=0; i<5; i++)
                    if (!minmea_isfield(field[1+i]))
                        goto end;

                char *buf = va_arg(ap, char *);
                memcpy(buf, field+1, 5);
                buf[5] = '\0';
            } break;

            case 'D': { // Date (int, int, int), -1 if empty.
                struct minmea_date *date = va_arg(ap, struct minmea_date *);

                int d = -1, m = -1, y = -1;
                // Always six digits.
                for (int i=0; i<6; i++)
                    if (!isdigit((unsigned char) field[i]))
                        goto end_D;

                d = strtol((char[]) {field[0], field[1], '\0'}, NULL, 10);
                m = strtol((char[]) {field[2], field[3], '\0'}, NULL, 10);
                y = strtol((char[]) {field[4], field[5], '\0'}, NULL, 10);

            end_D:
                date->day = d;
                date->month = m;
                date->year = y;
            } break;

            case 'T': { // Time (int, int, int, int), -1 if empty.
                struct minmea_time *time = va_arg(ap, struct minmea_time *);

                int h = -1, i = -1, s = -1, u = -1;
                // Minimum required: integer time.
                for (int i=0; i<6; i++)
                    if (!isdigit((unsigned char) field[i]))
                        goto end_T;

                h = strtol((char[]) {field[0], field[1], '\0'}, NULL, 10);
                i = strtol((char[]) {field[2], field[3], '\0'}, NULL, 10);
                s = strtol((char[]) {field[4], field[5], '\0'}, NULL, 10);
                field += 6;

                // Extra: fractional time. Saved as microseconds.
                if (*field++ == '.') {
                    int value = 0;
                    int scale = 1000000;
                    while (isdigit((unsigned char) *field) && scale > 1) {
                        value = (value * 10) + (*field++ - '0');
                        scale /= 10;
                    }
                    u = value * scale;
                } else {
                    u = 0;
                }

            end_T:
                time->hours = h;
                time->minutes = i;
                time->seconds = s;
                time->microseconds = u;
            } break;

            case '_': { // Ignore the field.
            } break;

            default: { // Unknown.
                goto end;
            } break;
        }

        // Advance to next field.
        next_field();
    }

    result = true;

end:
    va_end(ap);
    return result;
}

enum minmea_type minmea_type(const char *sentence)
{
    if (!minmea_check(sentence))
        return MINMEA_INVALID;

    char type[6];
    if (!minmea_scan(sentence, "t", type))
        return MINMEA_INVALID;

    if (!strcmp(type, "GPRMC"))
        return MINMEA_GPRMC;
    if (!strcmp(type, "GPGGA"))
        return MINMEA_GPGGA;
    if (!strcmp(type, "GPGSA"))
        return MINMEA_GPGSA;

    return MINMEA_UNKNOWN;
}

bool minmea_parse_gprmc(struct minmea_gprmc *frame, const char *sentence)
{
    // $GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
    char type[6];
    char validity;
    int latitude_direction;
    int longitude_direction;
    int variation_direction;
    if (!minmea_scan(sentence, "tTcfdfdffDfd",
            type,
            &frame->time,
            &validity,
            &frame->latitude, &frame->latitude_scale, &latitude_direction,
            &frame->longitude, &frame->longitude_scale, &longitude_direction,
            &frame->speed, &frame->speed_scale,
            &frame->course, &frame->course_scale,
            &frame->date,
            &frame->variation, &frame->variation_scale, &variation_direction))
        return false;
    if (strcmp(type, "GPRMC"))
        return false;

    frame->valid = (validity == 'A');
    frame->latitude *= latitude_direction;
    frame->longitude *= longitude_direction;
    frame->variation *= variation_direction;

    return true;
}

bool minmea_parse_gpgga(struct minmea_gpgga *frame, const char *sentence)
{
    // $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
    char type[6];
    int latitude_direction;
    int longitude_direction;

    if (!minmea_scan(sentence, "tTfdfdiiffcfci_",
            type,
            &frame->time,
            &frame->latitude, &frame->latitude_scale, &latitude_direction,
            &frame->longitude, &frame->longitude_scale, &longitude_direction,
            &frame->fix_quality,
            &frame->satellites_tracked,
            &frame->hdop, &frame->hdop_scale,
            &frame->altitude, &frame->altitude_scale, &frame->altitude_units,
            &frame->height, &frame->height_scale, &frame->height_units,
            &frame->dgps_age))
        return false;
    if (strcmp(type, "GPGGA"))
        return false;

    frame->latitude *= latitude_direction;
    frame->longitude *= longitude_direction;

    return true;
}

bool minmea_parse_gpgsa(struct minmea_gpgsa *frame, const char *sentence)
{
    // $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39
    char type[6];

    if (!minmea_scan(sentence, "tciiiiiiiiiiiiifff",
            type,
            &frame->mode,
            &frame->fix_type,
            &frame->sat1,
            &frame->sat2,
            &frame->sat3,
            &frame->sat4,
            &frame->sat5,
            &frame->sat6,
            &frame->sat7,
            &frame->sat8,
            &frame->sat9,
            &frame->sat10,
            &frame->sat11,
            &frame->sat12,
            &frame->pdop,
            &frame->pdop_scale,
            &frame->hdop,
            &frame->hdop_scale,
            &frame->vdop,
            &frame->vdop_scale
            )){
        return false;
    }
    if (strcmp(type, "GPGSA"))
        return false;

    if (frame->mode == 'A')
        frame->mode = GPGSA_MODE_AUTO;
    else if (frame->mode == 'M')
        frame->mode = GPGSA_MODE_FORCED;
    else
        return false;

    if (frame->fix_type == 1)
        frame->fix_type = GPGSA_FIX_NONE;
    else if (frame->fix_type == 2)
        frame->fix_type = GPGSA_FIX_2D;
    else if (frame->fix_type == 3)
        frame->fix_type = GPGSA_FIX_3D;
    else
        return false;

    return true;
}

int minmea_gettimeofday(struct timeval *tv, const struct minmea_date *date, const struct minmea_time *time)
{
    if (date->year == -1 || time->hours == -1)
        return -1;

    struct tm tm;
    tm.tm_year = 2000 + date->year - 1900;
    tm.tm_mon = date->month - 1;
    tm.tm_mday = date->day;
    tm.tm_hour = time->hours;
    tm.tm_min = time->minutes;
    tm.tm_sec = time->seconds;
    tm.tm_isdst = 0;
    time_t timestamp = timegm(&tm);

    if (timestamp != -1) {
        tv->tv_sec = timestamp;
        tv->tv_usec = time->microseconds;
        return 0;
    } else {
        return -1;
    }
}

/* vim: set ts=4 sw=4 et: */
