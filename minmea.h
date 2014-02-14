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

/**
 * Convert GPS UTC date/time representation to a UNIX timestamp.
 */
int minmea_gettimeofday(struct timeval *tv, const struct minmea_date *date, const struct minmea_time *time);

/**
 * Rescale signed integer value to a different full-scale value, with rounding.
 * The "from" value in the numerator cancels out with the denominator, leaving
 * just 1/2 - which makes integer truncation act as rounding. Returns zero for
 * invalid values.
 */
static inline int minmea_rescale(int value, int from, int to)
{
    if (from == 0)
        return 0;
    if (from == to)
        return value;
    return (value * to + from / 2) / from;
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
