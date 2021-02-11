#pragma once

#include <furi.h>
#include <assets_icons.h>

typedef int32_t (*FlipperApplication)(void*);

typedef struct {
    const FlipperApplication app;
    const char* name;
    const size_t stack_size;
    const IconName icon;
} FuriApplication;

/* Services list
 * Spawned on startup
 */
extern const FuriApplication FLIPPER_SERVICES[];
extern const size_t FLIPPER_SERVICES_COUNT;

/* Apps list
 * Spawned by app-loader
 */
extern const FuriApplication FLIPPER_APPS[];
extern const size_t FLIPPER_APPS_COUNT;

/* Plugins list
 * Spawned by app-loader
 */
extern const FuriApplication FLIPPER_PLUGINS[];
extern const size_t FLIPPER_PLUGINS_COUNT;
