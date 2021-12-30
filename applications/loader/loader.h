#pragma once

#include <furi/pubsub.h>
#include <stdbool.h>

typedef struct Loader Loader;

typedef enum {
    LoaderStatusOk,
    LoaderStatusErrorAppStarted,
    LoaderStatusErrorUnknownApp,
    LoaderStatusErrorInternal,
} LoaderStatus;

#define LOADER_BEFORE_APP_STARTED (void*)0
#define LOADER_AFTER_APP_FINISHED (void*)1

/** Start application
 * @param name - application name
 * @param args - application arguments
 * @retval true on success
 */
LoaderStatus loader_start(Loader* instance, const char* name, const char* args);

/** Lock application start
 * @retval true on success
 */
bool loader_lock(Loader* instance);

/** Unlock application start */
void loader_unlock(Loader* instance);

/** Get loader lock status */
bool loader_is_locked(Loader* instance);

/** Show primary loader */
void loader_show_menu();

/** Show primary loader */
void loader_update_menu();

/** Show primary loader */
FuriPubSub* loader_get_pubsub();
