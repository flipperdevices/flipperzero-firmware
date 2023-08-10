/* time_helper.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */
#include <string.h>
#include <lwip/apps/sntp.h>

#include "sdkconfig.h"
#include "esp_log.h"

#include "time_helper.h"

const static char* TAG = "Time Helper";

#define TIME_ZONE "PST-8"
/* NELEMS(x) number of elements
 * To determine the number of elements in the array, we can divide the total size of
 * the array by the size of the array element
 * See https://stackoverflow.com/questions/37538/how-do-i-determine-the-size-of-my-array-in-c
 **/
#define NELEMS(x)  ( (int)(sizeof(x) / sizeof((x)[0])) )
#define NTP_SERVER_LIST ( (char*[]) {                        \
                                     "pool.ntp.org",         \
                                     "time.nist.gov",        \
                                     "utcnist.colorado.edu"  \
                                     }                       \
                        )
/* #define NTP_SERVER_COUNT using NELEMS:
 *
 *  (int)(sizeof(NTP_SERVER_LIST) / sizeof(NTP_SERVER_LIST[0]))
 */
#define NTP_SERVER_COUNT NELEMS(NTP_SERVER_LIST)
char* ntpServerList[NTP_SERVER_COUNT] = NTP_SERVER_LIST;

/* our NTP server list is global info */
extern char* ntpServerList[NTP_SERVER_COUNT];


int set_time(void)
{
    /* we'll also return a result code of zero */
    int res = 0;
    int i = 0; /* counter for time servers */
    time_t interim_time;

    /* ideally, we'd like to set time from network,
     * but let's set a default time, just in case */
    struct tm timeinfo = {
        .tm_year = 2022 - 1900,
        .tm_mon = 11,
        .tm_mday = 15,
        .tm_hour = 3,
        .tm_min = 25,
        .tm_sec = 0
    };
    struct timeval now;

#ifndef NTP_SERVER_COUNT
    #define NTP_SERVER_COUNT 0
    char* ntpServerList[NTP_SERVER_COUNT];
#endif /* not defined: NTP_SERVER_COUNT */

#ifndef TIME_ZONE
    #define TIME_ZONE "PST-8"
#endif /* not defined: TIME_ZONE */


    /* set interim static time */
    interim_time = mktime(&timeinfo);
    now = (struct timeval){ .tv_sec = interim_time };
    settimeofday(&now, NULL);


    /* set timezone */
    setenv("TZ", TIME_ZONE, 1);
    tzset();

    if (NTP_SERVER_COUNT) {
        /* next, let's setup NTP time servers
         *
         * see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html#sntp-time-synchronization
         */
        sntp_setoperatingmode(SNTP_OPMODE_POLL);

        ESP_LOGI(TAG, "sntp_setservername:");
        for (i = 0; i < NTP_SERVER_COUNT; i++) {
            const char* thisServer = ntpServerList[i];
            if (strncmp(thisServer, "\x00", 1) == 0) {
                /* just in case we run out of NTP servers */
                break;
            }
            ESP_LOGI(TAG, "%s", thisServer);
            sntp_setservername(i, thisServer);
        }
        sntp_init();
        ESP_LOGI(TAG, "sntp_init done.");
    }
    else {
        ESP_LOGI(TAG, "No sntp time servers found.");
    }
    return res;
}

