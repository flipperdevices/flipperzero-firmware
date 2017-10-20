/*
 * Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wtype-limits"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <check.h>

#include "minmea.h"

static const char *valid_sentences_nochecksum[] = {
    "$GPTXT,xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
    NULL,
};

static const char *valid_sentences_checksum[] = {
    "$GPTXT,01,01,02,ANTSTATUS=INIT*25",
    "$GPRMC,,V,,,,,,,,,,N*53",
    "$GPVTG,,,,,,,,,N*30",
    "$GPGGA,,,,,,0,00,99.99,,,,,,*48",
    "$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30",
    "$GPGLL,,,,,,V,N*64",
    "$GPXTE,A,A,0.67,L,N*6F",
    "$GPXTE,A,A,0.67,L,N*6f",
    "$GPGGA,123204.00,5106.94086,N,01701.51680,E,1,06,3.86,127.9,M,40.5,M,,*51",
    "$GPGSA,A,3,02,08,09,05,04,26,,,,,,,4.92,3.86,3.05*00",
    "$GPGSV,4,1,13,02,28,259,33,04,12,212,27,05,34,305,30,07,79,138,*7F",
    "$GPGSV,4,2,13,08,51,203,30,09,45,215,28,10,69,197,19,13,47,081,*76",
    "$GPGSV,4,3,13,16,20,040,17,26,08,271,30,28,01,168,18,33,24,219,27*74",
    "$GPGSV,4,4,13,39,31,170,27*40",
    "$GPGLL,5106.94086,N,01701.51680,E,123204.00,A,A*63",
    "$GPRMC,123205.00,A,5106.94085,N,01701.51689,E,0.016,,280214,,,A*7B",
    "$GPVTG,,T,,M,0.016,N,0.030,K,A*27",
    "$GPGST,024603.00,3.2,6.6,4.7,47.3,5.8,5.6,22.0*58",
    "$GPZDA,160012.71,11,03,2004,-1,00*7D",
    NULL,
};

static const char *invalid_sentences[] = {
    "$GPTXT,xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
        "xxxxxxxxxxxxxxxxxxxxxxxxxxx",
    "$GPTXT,01,01,02,ANTSTATUS=INIT*26",
    "$GPRMC,,V,,,,,,,,,,N*532",
    "$GPVTG,,,,\xff,,,,,N*30",
    "$$GPGGA,,,,,,0,00,99.99,,,,,,*48",
    "GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30",
    "gps: $GPGLL,,,,,,V,N",
    "$GPXTE,A,A,0.67,L,N*6e",
    "$GPXTE,A,A,0.67,L,N*6g",
    NULL,
};

START_TEST(test_minmea_checksum)
{
    ck_assert_int_eq(minmea_checksum(""), 0x00);
    ck_assert_int_eq(minmea_checksum("$"), 0x00);
    ck_assert_int_eq(minmea_checksum("*"), 0x00);
    ck_assert_int_eq(minmea_checksum("$*"), 0x00);
    ck_assert_int_eq(minmea_checksum("$GPTXT,01,01,02,ANTSTATUS=INIT*25"), 0x25);
    ck_assert_int_eq(minmea_checksum("$GPTXT,01,01,02,ANTSTATUS=INIT"), 0x25);
    ck_assert_int_eq(minmea_checksum("GPTXT,01,01,02,ANTSTATUS=INIT*25"), 0x25);
    ck_assert_int_eq(minmea_checksum("GPTXT,01,01,02,ANTSTATUS=INIT"), 0x25);
    ck_assert_int_eq(minmea_checksum("$GPXTE,A,A,0.67,L,N*6F"), 0x6f);
    ck_assert_int_eq(minmea_checksum("GPXTE,A,A,0.67,L,N*6f"), 0x6f);
}
END_TEST

START_TEST(test_minmea_check)
{
    for (const char **sentence=valid_sentences_nochecksum; *sentence; sentence++) {
        ck_assert_msg(minmea_check(*sentence, false) == true, *sentence);
        ck_assert_msg(minmea_check(*sentence, true) == false, *sentence);
    }

    for (const char **sentence=valid_sentences_checksum; *sentence; sentence++) {
        ck_assert_msg(minmea_check(*sentence, false) == true, *sentence);
        ck_assert_msg(minmea_check(*sentence, true) == true, *sentence);
    }

    for (const char **sentence=invalid_sentences; *sentence; sentence++) {
        ck_assert_msg(minmea_check(*sentence, false) == false, *sentence);
        ck_assert_msg(minmea_check(*sentence, true) == false, *sentence);
    }
}
END_TEST

START_TEST(test_minmea_scan_c)
{
    char ch, extra;

    ck_assert(minmea_scan("A,123.45", "c", &ch) == true);
    ck_assert_int_eq(ch, 'A');

    ck_assert(minmea_scan("WUT,123.45", "c", &ch) == true);
    ck_assert_int_eq(ch, 'W');

    ck_assert(minmea_scan(",123.45", "c", &ch) == true);
    ck_assert_int_eq(ch, '\0');

    ck_assert(minmea_scan("A,B", "cc", &ch, &extra) == true);
    ck_assert_int_eq(ch, 'A');
    ck_assert_int_eq(extra, 'B');

    ck_assert(minmea_scan("C", "cc", &ch, &extra) == false);

    ck_assert(minmea_scan("D", "c;c", &ch, &extra) == true);
    ck_assert_int_eq(ch, 'D');
    ck_assert_int_eq(extra, '\0');
    ck_assert(minmea_scan("E,F", "c;c", &ch, &extra) == true);
    ck_assert_int_eq(ch, 'E');
    ck_assert_int_eq(extra, 'F');
}
END_TEST

START_TEST(test_minmea_scan_d)
{
    int direction;

    ck_assert(minmea_scan("K", "d", &direction) == false);

    ck_assert(minmea_scan("", "d", &direction) == true);
    ck_assert(minmea_scan(",foo", "d", &direction) == true);
    ck_assert_int_eq(direction, 0);
    ck_assert(minmea_scan("N", "d", &direction) == true);
    ck_assert_int_eq(direction, 1);
    ck_assert(minmea_scan("S,foo", "d", &direction) == true);
    ck_assert_int_eq(direction, -1);
    ck_assert(minmea_scan("W", "d", &direction) == true);
    ck_assert_int_eq(direction, -1);
    ck_assert(minmea_scan("E,foo", "d", &direction) == true);
    ck_assert_int_eq(direction, 1);
}
END_TEST

START_TEST(test_minmea_scan_f)
{
    struct minmea_float f;

    ck_assert(minmea_scan("-", "f", &f) == false);
    ck_assert(minmea_scan("10-", "f", &f) == false);
    ck_assert(minmea_scan("+-10", "f", &f) == false);
    ck_assert(minmea_scan("12..45", "f", &f) == false);
    ck_assert(minmea_scan("blah", "f", &f) == false);
    ck_assert(minmea_scan("12.3.4", "f", &f) == false);

    ck_assert(minmea_scan(",", "f", &f) == true);
    ck_assert_int_eq(f.scale, 0);
    ck_assert(minmea_scan("", "f", &f) == true);
    ck_assert_int_eq(f.scale, 0);

    ck_assert(minmea_scan("42", "f", &f) == true);
    ck_assert_int_eq(f.value, 42);
    ck_assert_int_eq(f.scale, 1);

    ck_assert(minmea_scan("15.345", "f", &f) == true);
    ck_assert_int_eq(f.value, 15345);
    ck_assert_int_eq(f.scale, 1000);

    ck_assert(minmea_scan("-1.23,V", "f", &f) == true);
    ck_assert_int_eq(f.value, -123);
    ck_assert_int_eq(f.scale, 100);

    /* the guaranteed range is 32 bits which translates to +-180 degrees
     * with 5 decimal digits. make sure we support that. */
    ck_assert(minmea_scan("18000.00000", "f", &f) == true);
    ck_assert_int_eq(f.value, 1800000000);
    ck_assert_int_eq(f.scale, 100000);
    ck_assert(minmea_scan("-18000.00000", "f", &f) == true);
    ck_assert_int_eq(f.value, -1800000000);
    ck_assert_int_eq(f.scale, 100000);

    if (sizeof(int_least32_t) == 4) {
        /* fits in 32 bits */
        ck_assert(minmea_scan("2147483647", "f", &f) == true);
        ck_assert_int_eq(f.value, 2147483647);
        ck_assert_int_eq(f.scale, 1);
        /* doesn't fit, truncate precision */
        ck_assert(minmea_scan("2147483.648", "f", &f) == true);
        ck_assert_int_eq(f.value, 214748364);
        ck_assert_int_eq(f.scale, 100);
        /* doesn't fit, bail out */
        ck_assert(minmea_scan("2147483648", "f", &f) == false);
    } else if (sizeof(int_least32_t) == 8) {
        /* Casting to int64_t is ugly, but otherwise we get this on machines with 32-bit int_least32_t:
         * error: comparison is always false due to limited range of data type [-Werror=type-limits] */
        /* fits in 64 bits */
        ck_assert(minmea_scan("9223372036854775807", "f", &f) == true);
        ck_assert_int_eq((int64_t) f.value, 9223372036854775807LL);
        ck_assert_int_eq(f.scale, 1);
        /* doesn't fit, truncate precision */
        ck_assert(minmea_scan("9223372036854775.808", "f", &f) == true);
        ck_assert_int_eq((int64_t) f.value, 922337203685477580LL);
        ck_assert_int_eq(f.scale, 100);
        /* doesn't fit, bail out */
        ck_assert(minmea_scan("9223372036854775808", "f", &f) == false);
    } else {
        ck_abort_msg("your platform is esoteric. please fix this unit test.");
    }

    /* optional f.values */
    ck_assert(minmea_scan("foo", "_;f", &f) == true);
    ck_assert_int_eq(f.scale, 0);
    ck_assert(minmea_scan("foo,", "_;f", &f) == true);
    ck_assert_int_eq(f.scale, 0);
    ck_assert(minmea_scan("foo,12.3", "_;f", &f) == true);
    ck_assert_int_eq(f.value, 123);
    ck_assert_int_eq(f.scale, 10);

    /* accept spaces at the start of the field. some modules do this, unfortunately. */
    ck_assert(minmea_scan(" -1.23,V", "f", &f) == true);
    ck_assert_int_eq(f.value, -123);
    ck_assert_int_eq(f.scale, 100);
    ck_assert(minmea_scan("     -4.56,V", "f", &f) == true);
    ck_assert_int_eq(f.value, -456);
    ck_assert_int_eq(f.scale, 100);
    ck_assert(minmea_scan("-3.33 ,V", "f", &f) == false);
    ck_assert(minmea_scan(" -3.33 ,V", "f", &f) == false);
    ck_assert(minmea_scan("-3. 33,V", "f", &f) == false);
    ck_assert(minmea_scan("0 .0,V", "f", &f) == false);
}
END_TEST

START_TEST(test_minmea_scan_i)
{
    int value, extra;

    // valid parses
    ck_assert(minmea_scan("14", "i", &value) == true);
    ck_assert_int_eq(value, 14);
    ck_assert(minmea_scan("-1234", "i", &value) == true);
    ck_assert_int_eq(value, -1234);

    // empty field
    ck_assert(minmea_scan("", "i", &value) == true);
    ck_assert_int_eq(value, 0);

    // invalid value
    ck_assert(minmea_scan("foo", "i", &value) == false);

    // missing field
    ck_assert(minmea_scan("41", "ii", &value, &extra) == false);

    /* optional values */
    ck_assert(minmea_scan("10", "i;i", &value, &extra) == true);
    ck_assert_int_eq(value, 10);
    ck_assert(minmea_scan("20,30", "i;i", &value, &extra) == true);
    ck_assert_int_eq(value, 20);
    ck_assert_int_eq(extra, 30);
    ck_assert(minmea_scan("42,foo", "i;i", &value, &extra) == false);
}
END_TEST

START_TEST(test_minmea_scan_s)
{
    char value[MINMEA_MAX_LENGTH];

    ck_assert(minmea_scan(",bar,baz", "s", value) == true);
    ck_assert_str_eq(value, "");
    ck_assert(minmea_scan("foo,bar,baz", "s", value) == true);
    ck_assert_str_eq(value, "foo");

    ck_assert(minmea_scan("dummy", "_;s", value) == true);
    ck_assert_str_eq(value, "");
    ck_assert(minmea_scan("dummy,foo", "_;s", value) == true);
    ck_assert_str_eq(value, "foo");
    ck_assert(minmea_scan("dummy,", "_;s", value) == true);
    ck_assert_str_eq(value, "");
}
END_TEST

START_TEST(test_minmea_scan_t)
{
    char buf[7];
    buf[sizeof(buf)-1] = 0x42;

    ck_assert(minmea_scan("$GPRM,foo,bar,baz", "t", buf) == false);
    ck_assert(minmea_scan("GPRMC,foo,bar,baz", "t", buf) == false);

    ck_assert(minmea_scan("$GPRMC,foo,bar,baz", "t", buf) == true);
    ck_assert_str_eq(buf, "GPRMC");

    ck_assert(buf[sizeof(buf)-1] == 0x42);
}
END_TEST

START_TEST(test_minmea_scan_D)
{
    struct minmea_date d;

    ck_assert(minmea_scan("$GPXXX,3112", "_D", &d) == false);
    ck_assert(minmea_scan("$GPXXX,foobar", "_D", &d) == false);

    ck_assert(minmea_scan("$GPXXX,311299", "_D", &d) == true);
    ck_assert_int_eq(d.day, 31);
    ck_assert_int_eq(d.month, 12);
    ck_assert_int_eq(d.year, 99);

    ck_assert(minmea_scan("$GPXXX,,,,,,,,,nope", "_D", &d) == true);
    ck_assert_int_eq(d.day, -1);
    ck_assert_int_eq(d.month, -1);
    ck_assert_int_eq(d.year, -1);
}
END_TEST

START_TEST(test_minmea_scan_T)
{
    struct minmea_time t;

    ck_assert(minmea_scan("$GPXXX,2359", "_T", &t) == false);
    ck_assert(minmea_scan("$GPXXX,foobar", "_T", &t) == false);

    ck_assert(minmea_scan("$GPXXX,235960", "_T", &t) == true);
    ck_assert_int_eq(t.hours, 23);
    ck_assert_int_eq(t.minutes, 59);
    ck_assert_int_eq(t.seconds, 60);
    ck_assert_int_eq(t.microseconds, 0);

    ck_assert(minmea_scan("$GPXXX,213700.001", "_T", &t) == true);
    ck_assert_int_eq(t.hours, 21);
    ck_assert_int_eq(t.minutes, 37);
    ck_assert_int_eq(t.seconds, 0);
    ck_assert_int_eq(t.microseconds, 1000);

    ck_assert(minmea_scan("$GPXXX,,,,,,,nope", "_T", &t) == true);
    ck_assert_int_eq(t.hours, -1);
    ck_assert_int_eq(t.minutes, -1);
    ck_assert_int_eq(t.seconds, -1);
    ck_assert_int_eq(t.microseconds, -1);
}
END_TEST

START_TEST(test_minmea_scan_complex1)
{
    const char *sentence = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n";
    char type[6];
    struct minmea_time t;
    struct minmea_float latitude; int latitude_direction;
    struct minmea_float longitude; int longitude_direction;
    int fix_quality;
    int satellites;
    struct minmea_float hdop;
    struct minmea_float altitude; char altitude_units;
    struct minmea_float height; char height_units;
    ck_assert(minmea_scan(sentence, "tTfdfdiiffcfc__",
        type,
        &t,
        &latitude, &latitude_direction,
        &longitude, &longitude_direction,
        &fix_quality,
        &satellites,
        &hdop,
        &altitude, &altitude_units,
        &height, &height_units) == true);
    ck_assert_str_eq(type, "GPGGA");
    ck_assert_int_eq(t.hours, 12);
    ck_assert_int_eq(t.minutes, 35);
    ck_assert_int_eq(t.seconds, 19);
    ck_assert_int_eq(latitude.value, 4807038);
    ck_assert_int_eq(latitude.scale, 1000);
    ck_assert_int_eq(latitude_direction, 1);
    ck_assert_int_eq(longitude.value, 1131000);
    ck_assert_int_eq(longitude.scale, 1000);
    ck_assert_int_eq(longitude_direction, 1);
    ck_assert_int_eq(fix_quality, 1);
    ck_assert_int_eq(satellites, 8);
    ck_assert_int_eq(hdop.value, 9);
    ck_assert_int_eq(hdop.scale, 10);
    ck_assert_int_eq(altitude.value, 5454);
    ck_assert_int_eq(altitude.scale, 10);
    ck_assert_int_eq(altitude_units, 'M');
    ck_assert_int_eq(height.value, 469);
    ck_assert_int_eq(height.scale, 10);
    ck_assert_int_eq(height_units, 'M');

}
END_TEST

START_TEST(test_minmea_scan_complex2)
{
    const char *sentence = "$GPBWC,081837,,,,,,T,,M,,N,*13";
    char type[6];
    struct minmea_time t;
    struct minmea_float latitude; int latitude_direction;
    struct minmea_float longitude; int longitude_direction;
    struct minmea_float bearing_true; char bearing_true_mark;
    struct minmea_float bearing_magnetic; char bearing_magnetic_mark;
    struct minmea_float distance; char distance_units;
    char name[MINMEA_MAX_LENGTH];
    ck_assert(minmea_scan(sentence, "tTfdfdfcfcfcs",
        type,
        &t,
        &latitude, &latitude_direction,
        &longitude, &longitude_direction,
        &bearing_true, &bearing_true_mark,
        &bearing_magnetic, &bearing_magnetic_mark,
        &distance, &distance_units,
        name) == true);
    ck_assert_str_eq(type, "GPBWC");
    ck_assert_int_eq(t.hours, 8);
    ck_assert_int_eq(t.minutes, 18);
    ck_assert_int_eq(t.seconds, 37);
    ck_assert_int_eq(latitude.scale, 0);
    ck_assert_int_eq(latitude_direction, 0);
    ck_assert_int_eq(longitude.scale, 0);
    ck_assert_int_eq(longitude_direction, 0);
    ck_assert_int_eq(bearing_true.scale, 0);
    ck_assert_int_eq(bearing_true_mark, 'T');
    ck_assert_int_eq(bearing_magnetic.scale, 0);
    ck_assert_int_eq(bearing_magnetic_mark, 'M');
    ck_assert_int_eq(distance.scale, 0);
    ck_assert_int_eq(distance_units, 'N');
    ck_assert_str_eq(name, "");
}
END_TEST

START_TEST(test_minmea_scan_complex3)
{
    const char *sentence = "$GPGST,024603.00,3.2,6.6,4.7,47.3,5.8,5.6,22.0*58";
    char type[6];
    struct minmea_time t;
    struct minmea_float rms_deviation;
    struct minmea_float semi_major_deviation;
    struct minmea_float semi_minor_deviation;
    struct minmea_float semi_major_orientation;
    struct minmea_float latitude_error_deviation;
    struct minmea_float longitude_error_deviation;
    struct minmea_float altitude_error_deviation;
    ck_assert(minmea_scan(sentence, "tTfffffff",
        type,
        &t,
        &rms_deviation,
        &semi_major_deviation,
        &semi_minor_deviation,
        &semi_major_orientation,
        &latitude_error_deviation,
        &longitude_error_deviation,
        &altitude_error_deviation) == true);
    ck_assert_str_eq(type, "GPGST");
    ck_assert_int_eq(t.hours, 2);
    ck_assert_int_eq(t.minutes, 46);
    ck_assert_int_eq(t.seconds, 3);
    ck_assert_int_eq(t.microseconds, 0);
    ck_assert_int_eq(rms_deviation.value, 32);
    ck_assert_int_eq(rms_deviation.scale, 10);
    ck_assert_int_eq(semi_major_deviation.value, 66);
    ck_assert_int_eq(semi_major_deviation.scale, 10);
    ck_assert_int_eq(semi_minor_deviation.value, 47);
    ck_assert_int_eq(semi_minor_deviation.scale, 10);
    ck_assert_int_eq(semi_major_orientation.value, 473);
    ck_assert_int_eq(semi_major_orientation.scale, 10);
    ck_assert_int_eq(latitude_error_deviation.value, 58);
    ck_assert_int_eq(latitude_error_deviation.scale, 10);
    ck_assert_int_eq(longitude_error_deviation.value, 56);
    ck_assert_int_eq(longitude_error_deviation.scale, 10);
    ck_assert_int_eq(altitude_error_deviation.value, 220);
    ck_assert_int_eq(altitude_error_deviation.scale, 10);
}
END_TEST

START_TEST(test_minmea_parse_rmc1)
{
    const char *sentence = "$GPRMC,081836.75,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E";
    struct minmea_sentence_rmc frame = {};
    static const struct minmea_sentence_rmc expected = {
        .time = { 8, 18, 36, 750000 },
        .valid = true,
        .latitude = { -375165, 100 },
        .longitude = { 1450736, 100 },
        .speed = { 0, 10 },
        .course = { 3600, 10 },
        .date = { 13, 9, 98 },
        .variation = { 113, 10 },
    };
    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == false);
    ck_assert(minmea_parse_rmc(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_rmc2)
{
    const char *sentence = "$GPRMC,,A,3751.65,N,14507.36,W,,,,,";
    struct minmea_sentence_rmc frame = {};
    static const struct minmea_sentence_rmc expected = {
        .time = { -1, -1, -1, -1 },
        .valid = true,
        .latitude = { 375165, 100 },
        .longitude = { -1450736, 100 },
        .date = { -1, -1, -1 },
    };
    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == false);
    ck_assert(minmea_parse_rmc(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_gga1)
{
    const char *sentence = "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47";
    struct minmea_sentence_gga frame = {};
    static const struct minmea_sentence_gga expected = {
        .time = { 12, 35, 19, 0 },
        .latitude = { 4807038, 1000 },
        .longitude = { 1131000, 1000 },
        .fix_quality = 1,
        .satellites_tracked = 8,
        .hdop = { 9, 10 },
        .altitude = { 5454, 10 },
        .altitude_units = 'M',
        .height = { 469, 10 },
        .height_units = 'M',
        .dgps_age = 0,
    };
    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == true);
    ck_assert(minmea_parse_gga(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_gst1)
{
    const char *sentence = "$GPGST,024603.00,3.2,6.6,4.7,47.3,5.8,5.6,22.0*58";
    struct minmea_sentence_gst frame = {};
    struct minmea_sentence_gst expected = {
        .time = { 2, 46, 3, 0 },
        .rms_deviation = { 32, 10 },
        .semi_major_deviation = { 66, 10 },
        .semi_minor_deviation = { 47, 10 },
        .semi_major_orientation = { 473, 10 },
        .latitude_error_deviation = { 58, 10 },
        .longitude_error_deviation = { 56, 10 },
        .altitude_error_deviation = { 220, 10 },
    };
    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == true);
    ck_assert(minmea_parse_gst(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_gsa1)
{
    const char *sentence = "$GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39";
    struct minmea_sentence_gsa frame = {};
    static const struct minmea_sentence_gsa expected = {
        .mode = MINMEA_GPGSA_MODE_AUTO,
        .fix_type = MINMEA_GPGSA_FIX_3D,
        .sats = { 4, 5, 0, 9, 12, 0, 0, 24, 0, 0, 0, 0 },
        .pdop = { 25, 10 },
        .hdop = { 13, 10 },
        .vdop = { 21, 10 },
    };
    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == true);
    ck_assert(minmea_parse_gsa(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_gll1)
{
    const char *sentence = "$GPGLL,3723.2475,N,12158.3416,W,161229.487,A,A*41";
    struct minmea_sentence_gll frame;
    struct minmea_sentence_gll expected;

    // clear structs before initialization to enable use of memcmp()
    // todo: add for other structs
    memset(&frame, 0, sizeof(frame));
    memset(&expected, 0, sizeof(expected));

    expected.latitude = (struct minmea_float){ 37232475, 10000 };
    expected.longitude = (struct minmea_float){ -121583416, 10000 };
    expected.time = (struct minmea_time){ 16, 12, 29, 487000 };
    expected.status = MINMEA_GLL_STATUS_DATA_VALID;
    expected.mode = MINMEA_FAA_MODE_AUTONOMOUS;

    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == true);
    ck_assert(minmea_parse_gll(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_gll2)
{
    const char *sentence = "$GPGLL,4916.45,N,12311.12,W,225444,A";
    struct minmea_sentence_gll frame = {};
    struct minmea_sentence_gll expected = {
        .latitude = { 491645, 100 },
        .longitude = { -1231112, 100 },
        .time = { 22, 54, 44 },
        .status = MINMEA_GLL_STATUS_DATA_VALID,
        .mode = 0,
    };

    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == false);
    ck_assert(minmea_parse_gll(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_gsv1)
{
    const char *sentence = "$GPGSV,3,3,11,22,42,067,42,24,14,311,43,27,05,244,00,,,,*4D";
    struct minmea_sentence_gsv frame = {};
    static const struct minmea_sentence_gsv expected = {
        .total_msgs = 3,
        .msg_nr = 3,
        .total_sats = 11,
        .sats = {
            {
                .nr = 22,
                .elevation = 42,
                .azimuth = 67,
                .snr = 42
            },
            {
                .nr = 24,
                .elevation = 14,
                .azimuth = 311,
                .snr = 43
            },
            {
                .nr = 27,
                .elevation = 5,
                .azimuth = 244,
                .snr = 0
            },
            {
                .nr = 0,
                .elevation = 0,
                .azimuth = 0,
                .snr = 0
            }
        }
    };
    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == true);
    ck_assert(minmea_parse_gsv(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_gsv2)
{
    const char *sentence = "$GPGSV,4,2,11,08,51,203,30,09,45,215,28*75";
    struct minmea_sentence_gsv frame = {};
    static const struct minmea_sentence_gsv expected = {
        .total_msgs = 4,
        .msg_nr = 2,
        .total_sats = 11,
        .sats = {
            {
                .nr = 8,
                .elevation = 51,
                .azimuth = 203,
                .snr = 30
            },
            {
                .nr = 9,
                .elevation = 45,
                .azimuth = 215,
                .snr = 28
            },
            {
                .nr = 0,
                .elevation = 0,
                .azimuth = 0,
                .snr = 0
            },
            {
                .nr = 0,
                .elevation = 0,
                .azimuth = 0,
                .snr = 0
            }
        }
    };
    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == true);
    ck_assert(minmea_parse_gsv(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_gsv3)
{
    const char *sentence = "$GPGSV,4,4,13,39,31,170,27*40";
    struct minmea_sentence_gsv frame = {};
    static const struct minmea_sentence_gsv expected = {
        .total_msgs = 4,
        .msg_nr = 4,
        .total_sats = 13,
        .sats = {
            {
                .nr = 39,
                .elevation = 31,
                .azimuth = 170,
                .snr = 27
            },
            {
                .nr = 0,
                .elevation = 0,
                .azimuth = 0,
                .snr = 0
            },
            {
                .nr = 0,
                .elevation = 0,
                .azimuth = 0,
                .snr = 0
            },
            {
                .nr = 0,
                .elevation = 0,
                .azimuth = 0,
                .snr = 0
            }
        }
    };
    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == true);
    ck_assert(minmea_parse_gsv(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_gsv4)
{
    const char *sentence = "$GPGSV,4,4,13*7B";
    struct minmea_sentence_gsv frame = {};
    static const struct minmea_sentence_gsv expected = {
        .total_msgs = 4,
        .msg_nr = 4,
        .total_sats = 13,
        .sats = {
            {
                .nr = 0,
                .elevation = 0,
                .azimuth = 0,
                .snr = 0
            },
            {
                .nr = 0,
                .elevation = 0,
                .azimuth = 0,
                .snr = 0
            },
            {
                .nr = 0,
                .elevation = 0,
                .azimuth = 0,
                .snr = 0
            },
            {
                .nr = 0,
                .elevation = 0,
                .azimuth = 0,
                .snr = 0
            }
        }
    };
    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == true);
    ck_assert(minmea_parse_gsv(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_gsv5)
{
    const char *sentence = "$GPGSV,4,1,13,02,28,259,33,04,12,212,27,05,34,305,30,07,79,138,*7F";
    struct minmea_sentence_gsv frame = {};
    static const struct minmea_sentence_gsv expected = {
        .total_msgs = 4,
        .msg_nr = 1,
        .total_sats = 13,
        .sats = {
            {
                .nr = 2,
                .elevation = 28,
                .azimuth = 259,
                .snr = 33
            },
            {
                .nr = 4,
                .elevation = 12,
                .azimuth = 212,
                .snr = 27
            },
            {
                .nr = 5,
                .elevation = 34,
                .azimuth = 305,
                .snr = 30
            },
            {
                .nr = 7,
                .elevation = 79,
                .azimuth = 138,
                .snr = 0
            }
        }
    };
    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == true);
    ck_assert(minmea_parse_gsv(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST


START_TEST(test_minmea_parse_vtg1)
{
    const char *sentence = "$GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48";
    // clear structs before initialization to enable use of memcmp()
    struct minmea_sentence_vtg frame = {};
    struct minmea_sentence_vtg expected = {};

    expected = (struct minmea_sentence_vtg){
        .true_track_degrees = { 547, 10 },
        .magnetic_track_degrees = { 344, 10 },
        .speed_knots = { 55, 10 },
        .speed_kph = { 102, 10 },
        .faa_mode = 0,
    };

    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == true);
    ck_assert(minmea_parse_vtg(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_vtg2)
{
    const char *sentence = "$GPVTG,188.36,T,,M,0.820,N,1.519,K,A*3F";
    // clear structs before initialization to enable use of memcmp()
    struct minmea_sentence_vtg frame = {};
    struct minmea_sentence_vtg expected = {};

    expected = (struct minmea_sentence_vtg){
        .true_track_degrees = { 18836, 100 },
        .magnetic_track_degrees = { 0, 0 },
        .speed_knots = { 820, 1000 },
        .speed_kph = { 1519, 1000 },
        .faa_mode = MINMEA_FAA_MODE_AUTONOMOUS,
    };

    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == true);
    ck_assert(minmea_parse_vtg(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_parse_zda1)
{
    const char *sentence = "$GPZDA,160012.71,11,03,2004,-1,00*7D";
    struct minmea_sentence_zda frame = {};
    struct minmea_sentence_zda expected = {};

    expected = (struct minmea_sentence_zda) {
        .time = { 16, 0, 12, 710000 },
        .date = { 11, 3, 2004 },
        .hour_offset = -1,
        .minute_offset = 0,
    };

    ck_assert(minmea_check(sentence, false) == true);
    ck_assert(minmea_check(sentence, true) == true);
    ck_assert(minmea_parse_zda(&frame, sentence) == true);
    ck_assert(!memcmp(&frame, &expected, sizeof(frame)));
}
END_TEST

START_TEST(test_minmea_usage1)
{
    const char *sentences[] = {
        "$GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62",
        "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47",
        "$GNGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1",
        "$GPGLL,3723.2475,N,12158.3416,W,161229.487,A,A*41",
        "$GPGST,024603.00,3.2,6.6,4.7,47.3,5.8,5.6,22.0*58",
        "$GPVTG,096.5,T,083.5,M,0.0,N,0.0,K,D*22",
        NULL,
    };

    for (const char **sentence=sentences; *sentence; sentence++) {
        switch (minmea_sentence_id(*sentence, false)) {
            case MINMEA_SENTENCE_RMC: {
                struct minmea_sentence_rmc frame;
                ck_assert(minmea_parse_rmc(&frame, *sentence) == true);
            } break;

            case MINMEA_SENTENCE_GGA: {
                struct minmea_sentence_gga frame;
                ck_assert(minmea_parse_gga(&frame, *sentence) == true);
            } break;

            case MINMEA_SENTENCE_GSA: {
                struct minmea_sentence_gsa frame;
                ck_assert(minmea_parse_gsa(&frame, *sentence) == true);
            } break;

            case MINMEA_SENTENCE_GLL: {
                struct minmea_sentence_gll frame;
                ck_assert(minmea_parse_gll(&frame, *sentence) == true);
            } break;

            case MINMEA_SENTENCE_GST: {
                struct minmea_sentence_gst frame;
                ck_assert(minmea_parse_gst(&frame, *sentence) == true);
            } break;

            case MINMEA_SENTENCE_VTG: {
                struct minmea_sentence_vtg frame;
                ck_assert(minmea_parse_vtg(&frame, *sentence) == true);
            } break;

            default: {
            } break;
        }

        char talker[3];
        char expected[3] = { (*sentence)[1], (*sentence)[2], '\0' };
        ck_assert(minmea_talker_id(talker, *sentence) == true);
        ck_assert_str_eq(talker, expected);
    }
}
END_TEST

START_TEST(test_minmea_gettime)
{
    struct minmea_date d = { 14, 2, 14 };
    struct minmea_time t = { 13, 0, 9, 123456 };
    struct timespec ts;
    ck_assert(minmea_gettime(&ts, &d, &t) == 0);
    ck_assert_int_eq(ts.tv_sec, 1392382809);
    ck_assert_int_eq(ts.tv_nsec, 123456000);

    d.year = -1;
    ck_assert(minmea_gettime(&ts, &d, &t) != 0);
    d.year = 14;

    t.hours = -1;
    ck_assert(minmea_gettime(&ts, &d, &t) != 0);
    t.hours = 13;

    /* two digit year conversions */
    d.year = 80;
    ck_assert(minmea_gettime(&ts, &d, &t) == 0);
    ck_assert_int_eq(ts.tv_sec, 319381209);      /* 1980 */
    d.year = 37;
    ck_assert(minmea_gettime(&ts, &d, &t) == 0);
    ck_assert_int_eq(ts.tv_sec, 2118229209);     /* 2037 */
    /* skip >= 2038 tests on 32-bit time_t platforms */
    if (sizeof(time_t) == sizeof(int64_t)) {
        d.year = 79;
        ck_assert(minmea_gettime(&ts, &d, &t) == 0);
        ck_assert_int_eq(ts.tv_sec, 3443605209); /* 2079 */
    }

    /* four digit year conversions */
    d.year = 1979;
    ck_assert(minmea_gettime(&ts, &d, &t) == 0);
    ck_assert_int_eq(ts.tv_sec, 287845209);
    d.year = 1980;
    ck_assert(minmea_gettime(&ts, &d, &t) == 0);
    ck_assert_int_eq(ts.tv_sec, 319381209);
    d.year = 2037;
    ck_assert(minmea_gettime(&ts, &d, &t) == 0);
    ck_assert_int_eq(ts.tv_sec, 2118229209);
    /* skip >= 2038 tests on 32-bit time_t platforms */
    if (sizeof(time_t) == sizeof(int64_t)) {
        d.year = 2079;
        ck_assert(minmea_gettime(&ts, &d, &t) == 0);
        ck_assert_int_eq(ts.tv_sec, 3443605209);
        d.year = 2080;
        ck_assert(minmea_gettime(&ts, &d, &t) == 0);
        ck_assert_int_eq(ts.tv_sec, 3475141209);
    }
}
END_TEST

START_TEST(test_minmea_rescale)
{
    /* basic and edge cases. */
    ck_assert_int_eq(minmea_rescale(&(struct minmea_float) { 42, 0 }, 3), 0);
    ck_assert_int_eq(minmea_rescale(&(struct minmea_float) { 1234, 10 }, 1), 123);
    ck_assert_int_eq(minmea_rescale(&(struct minmea_float) { 1235, 10 }, 1), 124);
    ck_assert_int_eq(minmea_rescale(&(struct minmea_float) { 1234, 10 }, 1000), 123400);

    /* round towards zero. */
    ck_assert_int_eq(minmea_rescale(&(struct minmea_float) { -1234, 10 }, 1), -123);
    ck_assert_int_eq(minmea_rescale(&(struct minmea_float) { -1235, 10 }, 1), -124);
    ck_assert_int_eq(minmea_rescale(&(struct minmea_float) { -1236, 10 }, 1), -124);

    /* shouldn't overflow on large numbers. */
    ck_assert_int_eq(minmea_rescale(&(struct minmea_float) { 510693608, 100000 }, 10000), 51069361);
}
END_TEST

/* The float values used in tests should be exactly representable under IEEE754;
 * false negatives will occur otherwise. */

#define assert_float_eq(x, y) ck_assert(fabsf((x) - (y)) <= 0.0f)

START_TEST(test_minmea_float)
{
    ck_assert(isnan(minmea_tofloat(&(struct minmea_float) { 42, 0 })));
    assert_float_eq(minmea_tofloat(&(struct minmea_float) { 7, 1 }), 7.0f);
    assert_float_eq(minmea_tofloat(&(struct minmea_float) { -200, 100 }), -2.0f);
    assert_float_eq(minmea_tofloat(&(struct minmea_float) { 15, 10 }), 1.5f);
}
END_TEST

START_TEST(test_minmea_coord)
{
    ck_assert(isnan(minmea_tocoord(&(struct minmea_float) { 42, 0 })));
    assert_float_eq(minmea_tocoord(&(struct minmea_float) { 4200, 1 }), 42.0f);
    assert_float_eq(minmea_tocoord(&(struct minmea_float) { 420000, 100 }), 42.0f);
    assert_float_eq(minmea_tocoord(&(struct minmea_float) { 423000, 100 }), 42.5f);
}
END_TEST

static Suite *minmea_suite(void)
{
    Suite *s = suite_create ("minmea");

    TCase *tc_checksum = tcase_create("minmea_checksum");
    tcase_add_test(tc_checksum, test_minmea_checksum);
    suite_add_tcase(s, tc_checksum);

    TCase *tc_check = tcase_create("minmea_check");
    tcase_add_test(tc_check, test_minmea_check);
    suite_add_tcase(s, tc_check);

    TCase *tc_scan = tcase_create("minmea_scan");
    tcase_add_test(tc_scan, test_minmea_scan_c);
    tcase_add_test(tc_scan, test_minmea_scan_d);
    tcase_add_test(tc_scan, test_minmea_scan_f);
    tcase_add_test(tc_scan, test_minmea_scan_i);
    tcase_add_test(tc_scan, test_minmea_scan_s);
    tcase_add_test(tc_scan, test_minmea_scan_t);
    tcase_add_test(tc_scan, test_minmea_scan_D);
    tcase_add_test(tc_scan, test_minmea_scan_T);
    tcase_add_test(tc_scan, test_minmea_scan_complex1);
    tcase_add_test(tc_scan, test_minmea_scan_complex2);
    tcase_add_test(tc_scan, test_minmea_scan_complex3);
    suite_add_tcase(s, tc_scan);

    TCase *tc_parse = tcase_create("minmea_parse");
    tcase_add_test(tc_parse, test_minmea_parse_rmc1);
    tcase_add_test(tc_parse, test_minmea_parse_rmc2);
    tcase_add_test(tc_parse, test_minmea_parse_gga1);
    tcase_add_test(tc_parse, test_minmea_parse_gsa1);
    tcase_add_test(tc_parse, test_minmea_parse_gll1);
    tcase_add_test(tc_parse, test_minmea_parse_gll2);
    tcase_add_test(tc_parse, test_minmea_parse_gst1);
    tcase_add_test(tc_parse, test_minmea_parse_gsv1);
    tcase_add_test(tc_parse, test_minmea_parse_gsv2);
    tcase_add_test(tc_parse, test_minmea_parse_gsv3);
    tcase_add_test(tc_parse, test_minmea_parse_gsv4);
    tcase_add_test(tc_parse, test_minmea_parse_gsv5);
    tcase_add_test(tc_parse, test_minmea_parse_vtg1);
    tcase_add_test(tc_parse, test_minmea_parse_vtg2);
    tcase_add_test(tc_parse, test_minmea_parse_zda1);
    suite_add_tcase(s, tc_parse);

    TCase *tc_usage = tcase_create("minmea_usage");
    tcase_add_test(tc_usage, test_minmea_usage1);
    suite_add_tcase(s, tc_usage);

    TCase *tc_utils = tcase_create("minmea_utils");
    tcase_add_test(tc_utils, test_minmea_gettime);
    tcase_add_test(tc_utils, test_minmea_rescale);
    tcase_add_test(tc_utils, test_minmea_float);
    tcase_add_test(tc_utils, test_minmea_coord);
    suite_add_tcase(s, tc_utils);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s = minmea_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/* vim: set ts=4 sw=4 et: */
