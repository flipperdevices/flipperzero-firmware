#pragma once

#include "cmsis_os.h"
#include <stdbool.h>

typedef void(*FlipperApplication)(void*);

// Furiac application handler
typedef struct {
    TaskHandle_t handler;
} FuriApp;

typedef struct {

} FuriRecord;

/*
Simply starts application.
It call app entrypoint with param passed as argument.
Useful for daemon applications and pop-up.
*/
FuriApp furiac_start(FlipperApplication app, const char* name, void* param);

/*
Swtich to other application.
FURI stop current app, call app entrypoint with param passed as
argument and save current application entrypoint to prev field
in current application registry.
Useful for UI or "active" application.
*/
FuriApp furiac_switch(FlipperApplication app, char* name, void* param);

/*
Stop current application
(stop thread and clear application's stack), start application
from prev entry in current application registry, cleanup current
application registry.
*/
bool furiac_exit(void* param);

/*
Stop specified app without returning to prev application.
*/
bool furiac_kill(FuriApp app);

/*
Creates named FURI record.
Returns NULL if registry have not enough memory for creating.
*/
FuriRecord* furi_create(char* name);
