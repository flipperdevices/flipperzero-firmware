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

            default: {
            } break;
        }
    }

    return 0;
}

/* vim: set ts=4 sw=4 et: */
