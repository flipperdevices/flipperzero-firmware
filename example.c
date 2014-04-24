/*
 * Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
 * This program is free software. It comes without any warranty, to the extent
 * permitted by applicable law. You can redistribute it and/or modify it under
 * the terms of the Do What The Fuck You Want To Public License, Version 2, as
 * published by Sam Hocevar. See the COPYING file for more details.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "minmea.h"

int main()
{
    char line[MINMEA_MAX_LENGTH];
    while (fgets(line, sizeof(line), stdin) != NULL) {
        printf("%s", line);
        switch (minmea_sentence_id(line)) {
            case MINMEA_SENTENCE_RMC: {
                struct minmea_sentence_rmc frame;
                if (minmea_parse_rmc(&frame, line)) {
                    printf("+++ raw coordinates and speed: (%d/%d,%d/%d) %d/%d\n",
                            frame.latitude, frame.latitude_scale,
                            frame.longitude, frame.longitude_scale,
                            frame.speed, frame.speed_scale);
                    printf("+++ fixed-point coordinates and speed scaled to three decimal places: (%d,%d) %d\n",
                            minmea_rescale(frame.latitude, frame.latitude_scale, 1000),
                            minmea_rescale(frame.longitude, frame.longitude_scale, 1000),
                            minmea_rescale(frame.speed, frame.speed_scale, 1000));
                    printf("+++ floating point degree coordinates and speed: (%f,%f) %f\n",
                            minmea_coord(frame.latitude, frame.latitude_scale),
                            minmea_coord(frame.longitude, frame.longitude_scale),
                            minmea_float(frame.speed, frame.speed_scale));
                }
            } break;

            case MINMEA_SENTENCE_GGA: {
                struct minmea_sentence_gga frame;
                if (minmea_parse_gga(&frame, line)) {
                    printf("$GPGGA: fix quality: %d\n", frame.fix_quality);
                }
            } break;

            case MINMEA_SENTENCE_GST: {
                struct minmea_sentence_gst frame;
                if (minmea_parse_gst(&frame, line)) {
                    printf("+++ raw latitude,longitude and altitude error deviation: (%d/%d,%d/%d,%d/%d)\n",
                            frame.latitude_error_deviation, frame.latitude_error_deviation_scale,
                            frame.longitude_error_deviation, frame.longitude_error_deviation_scale,
                            frame.altitude_error_deviation, frame.altitude_error_deviation_scale);
                    printf("+++ fixed point latitude,longitude and altitude error deviation \
                           scaled to one decimal place: (%d,%d,%d)\n",
                            minmea_rescale(frame.latitude_error_deviation, frame.latitude_error_deviation_scale,10),
                            minmea_rescale(frame.longitude_error_deviation, frame.longitude_error_deviation_scale,10),
                            minmea_rescale(frame.altitude_error_deviation, frame.altitude_error_deviation_scale,10));
                    printf("+++ floating point degree latitude,longitude and altitude error deviation: (%f,%f,%f)",
                            minmea_coord(frame.latitude_error_deviation, frame.latitude_error_deviation_scale),
                            minmea_coord(frame.longitude_error_deviation, frame.longitude_error_deviation_scale),
                            minmea_coord(frame.altitude_error_deviation, frame.altitude_error_deviation_scale));
                }
            } break;

            case MINMEA_SENTENCE_GSV: {
                struct minmea_sentence_gsv frame;
                if (minmea_parse_gsv(&frame, line)) {
                    printf("$GPGSV: message %d of %d\n", frame.msg_nr, frame.total_msgs);
                    printf("$GPGSV: sattelites in view: %d\n", frame.total_sats);
                    for (int i = 0; i < 4; i++)
                        printf("$GPGSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n",
                            frame.sats[i].nr,
                            frame.sats[i].elevation,
                            frame.sats[i].azimuth,
                            frame.sats[i].snr);
                }
            } break;

            default: {
            } break;
        }
    }

    return 0;
}

/* vim: set ts=4 sw=4 et: */
