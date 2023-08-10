/* main.c
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

#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <applibs/log.h>
#include <applibs/gpio.h>
#include <wolfssl/wolfcrypt/settings.h>
#ifndef WOLFSSL_USER_SETTINGS
    #error "user_settings.h not included"
#endif
#include <server.h>
#include <client.h>

/*
 * The following #include imports a "template appliance" definition. This app
 * comes with multiple implementations of the template appliance, each in a
 * separate directory, which allow the code to run unchanged on different
 * hardware.
 *
 * By default, this app targets hardware that follows the MT3620-mini Reference
 * Development Board (MDB) specification, such as the MT3620-mini Dev Kit from
 * Seeed Studio.
 *
 * To target different hardware, you'll need to update CMakeLists.txt.
 * For example, to target the Avnet MT3620 Starter Kit, make this update:
 *      azsphere_target_hardware_definition(${PROJECT_NAME}
 *      TARGET_DIRECTORY "HardwareDefinitions/avnet_mt3620_sk"
 *      TARGET_DEFINITION "template_appliance.json")
 *
 * See https://aka.ms/AzureSphereHardwareDefinitions for more details.
 */
#include <hw/template_appliance.h>

typedef enum {
    ExitCode_Success = 0,

    ExitCode_Main_Led = 1
} ExitCode;

int main(void)
{
    Log_Debug(
        "\nVisit https://github.com/Azure/azure-sphere-samples for other"
        " examples.\n");
    int ret;

    int fd = GPIO_OpenAsOutput(WOLF_AZSPHERE, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (fd < 0) {
        Log_Debug(
            "Error opening GPIO: %s (%d). Check that app_manifest.json includes"
            " the GPIO used.\n",
            strerror(errno), errno);
        return ExitCode_Main_Led;
    }

    const struct timespec sleepTime = {.tv_sec = 1, .tv_nsec = 0};

    printf("Launching the server...\n");
    ret = azsphere_server_app();
    printf("ret azsphere_server_app = %d\n", ret);

    /* if server exists continually blink the red LED indicating server needs
     * to be rebooted */
    while (true) {
        GPIO_SetValue(fd, GPIO_Value_Low);
        nanosleep(&sleepTime, NULL);
        GPIO_SetValue(fd, GPIO_Value_High);
        nanosleep(&sleepTime, NULL);
    }
}
