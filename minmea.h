/*
 * Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#ifndef MINMEA_H
#define MINMEA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>

#define MINMEA_MAX_LENGTH 80

enum minmea_type {
    MINMEA_INVALID = -1,
    MINMEA_UNKNOWN = 0,
    MINMEA_GPRMC,
    MINMEA_GPGGA,
    MINMEA_GPGSA,
};

struct minmea_date {
    int day;
    int month;
    int year;
};

struct minmea_time {
    int hours;
    int minutes;
    int seconds;
    int microseconds;
};

struct minmea_gprmc {
    struct minmea_time time;
    bool valid;
    int latitude, latitude_scale;
    int longitude, longitude_scale;
    int speed, speed_scale;
    int course, course_scale;
    struct minmea_date date;
    int variation, variation_scale;
};

struct minmea_gpgga {
    struct minmea_time time;
    int latitude, latitude_scale;
    int longitude, longitude_scale;
    int fix_quality;
    int satellites_tracked;
    int hdop, hdop_scale;
    int altitude, altitude_scale; char altitude_units;
    int height, height_scale; char height_units;
    int dgps_age;
};

enum minmea_gpgsa_mode {
    MINMEA_GPGSA_MODE_AUTO = 'A',
    MINMEA_GPGSA_MODE_FORCED = 'M',
};

enum minmea_gpgsa_fix_type {
    MINMEA_GPGSA_FIX_NONE = 1,
    MINMEA_GPGSA_FIX_2D = 2,
    MINMEA_GPGSA_FIX_3D = 3,
};

struct minmea_gpgsa {
    char mode;
    int fix_type;
    int sats[12];
    int pdop, pdop_scale;
    int hdop, hdop_scale;
    int vdop, vdop_scale;
};

/**
 * Check sequence validity and checksum. Returns true for valid sequences.
 */
bool minmea_check(const char *sentence);

/**
 * Determine sequence type.
 */
enum minmea_type minmea_type(const char *sequence);

/**
 * Scanf-like processor for NMEA sentences. Supports the following formats:
 * c - single character (char *)
 * d - direction, returned as 1/-1, default 0 (int *)
 * f - fractional, returned as value + scale (int *, int *)
 * i - decimal, default zero (int *)
 * s - string (char *)
 * t - talker identifier and type (char *)
 * T - date/time stamp (int *, int *, int *)
 * Returns true on success. See library source code for details.
 */
bool minmea_scan(const char *sentence, const char *format, ...);

/*
 * Parse a specific type of frame. Return true on success.
 */
bool minmea_parse_gprmc(struct minmea_gprmc *frame, const char *sentence);
bool minmea_parse_gpgga(struct minmea_gpgga *frame, const char *sentence);
bool minmea_parse_gpgsa(struct minmea_gpgsa *frame, const char *sentence);

/**
 * Convert GPS UTC date/time representation to a UNIX timestamp.
 */
int minmea_gettimeofday(struct timeval *tv, const struct minmea_date *date, const struct minmea_time *time);

/**
 * Rescale a fixed-point value to a different scale. Rounds towards zero.
 */
static inline int minmea_rescale(int value, int from, int to)
{
    if (from == 0)
        return 0;
    if (from == to)
        return value;
    if (from > to)
        return (value + ((value > 0) - (value < 0)) * from/to/2) / (from/to);
    else
        return value * (to/from);
}

/**
 * Convert a fixed-point value to a floating-point value.
 * Returns NaN for "unknown" values.
 */
static inline float minmea_float(int value, int scale)
{
    if (scale == 0)
        return NAN;
    return (float) value / (float) scale;
}

/**
 * Convert a raw coordinate to a floating point DD.DDD... value.
 * Returns NaN for "unknown" values.
 */
static inline float minmea_coord(int value, int scale)
{
    if (scale == 0)
        return NAN;
    int degrees = value / (scale * 100);
    int minutes = value % (scale * 100);
    return (float) degrees + (float) minutes / (60 * scale);
}

#ifdef __cplusplus
}
#endif

#endif /* MINMEA_H */

/* vim: set ts=4 sw=4 et: */
