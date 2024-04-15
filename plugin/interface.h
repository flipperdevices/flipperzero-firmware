/**
 * @file plugin_interface.h
 * @brief Example plugin interface.
 *
 * Common interface between a plugin and host application
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <furi.h>

#define PLUGIN_APP_ID "plugin_wiegand"
#define PLUGIN_API_VERSION 1

typedef struct {
    const char* name;
    int (*count)(uint8_t, uint64_t);
    void (*description)(uint8_t, uint64_t, size_t, FuriString*);
} PluginWiegand;
