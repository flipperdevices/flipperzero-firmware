#pragma once

#include <furi.h>
#include <gui/icon.h>
#include <assets_icons.h>

typedef enum {
    FlipperApplicationFlagDefault = 0,
    FlipperApplicationFlagInsomniaSafe = (1 << 0),
} FlipperApplicationFlag;

typedef struct {
    const FuriThreadCallback app;
    const char* name;
    const char* appid;
    const size_t stack_size;
    const Icon* icon;
    const FlipperApplicationFlag flags;
    const char* link;
} FlipperApplication;

/* Apps list
 * Spawned by loader
 */
const FlipperApplication FLIPPER_APPS2[] = {

    {
        .name = "Clock",
    },

    {
        .name = "Applications",
    },

    {
        .name = "Sub-GHz",
    },

    {
        .name = "Sub-GHz Remote",
    },

    {
        .name = "Sub-GHz Playlist",
    },

    {
        .name = "125 kHz RFID",
    },

    {
        .name = "NFC",
    },

    {
        .name = "Infrared",
    },

    {
        .name = "IR Remote",
    },

    {
        .name = "GPIO",
    },

    {
        .name = "iButton",
    },

    {
        .name = "Bad USB",
    },

    {
        .name = "U2F",
    }};
const size_t FLIPPER_APPS_COUNT2 = COUNT_OF(FLIPPER_APPS2);
