/*
 * Copyright © 2014 Kosma Moczek <kosma@cloudyourcar.com>
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
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
        switch (minmea_type(line)) {
            case MINMEA_GPRMC: {
                struct minmea_gprmc frame;
                if (minmea_parse_gprmc(&frame, line)) {
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

            case MINMEA_GPGGA: {
                struct minmea_gpgga frame;
                if (minmea_parse_gpgga(&frame, line)) {
                    printf("$GPGGA: fix quality: %d\n", frame.fix_quality);
                }
            } break;

            default: {
            } break;
        }
    }

    return 0;
}

/* vim: ts=4 sw=4 et: */
