#pragma once

#include <furi.h>
#include <gui/icon.h>

typedef struct {
    const FuriThreadCallback app;
    const char* name;
    const size_t stack_size;
    const Icon* icon;
} FlipperApplication;

typedef void (*FlipperOnStartHook)(void);

/* Services list
 * Spawned on startup
 */
extern const FlipperApplication FLIPPER_SERVICES[];
extern const size_t FLIPPER_SERVICES_COUNT;

/* Apps list
 * Spawned by loader
 */
extern const FlipperApplication FLIPPER_APPS[];
extern const size_t FLIPPER_APPS_COUNT;

/* On system start hooks
 * Called by loader, after OS initialization complete
 */
extern const FlipperOnStartHook FLIPPER_ON_SYSTEM_START[];
extern const size_t FLIPPER_ON_SYSTEM_START_COUNT;

/* Plugins list
 * Spawned by loader
 */
extern const FlipperApplication FLIPPER_PLUGINS[];
extern const size_t FLIPPER_PLUGINS_COUNT;

/* Debug menu apps
 * Spawned by loader
 */
extern const FlipperApplication FLIPPER_DEBUG_APPS[];
extern const size_t FLIPPER_DEBUG_APPS_COUNT;

/* Desktop apps
 * For access from idle screen
 */
extern const FlipperApplication FLIPPER_WORLD;
extern const FlipperApplication FLIPPER_ARCHIVE;

/* Settings list
 * Spawned by loader
 */
extern const FlipperApplication FLIPPER_SETTINGS_APPS[];
extern const size_t FLIPPER_SETTINGS_APPS_COUNT;