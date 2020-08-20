#pragma once

#include "cmsis_os.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_TASK_RECORDS 8
#define MAX_RECORD_SUBSCRIBERS 8

/// application is just a function
typedef void(*FlipperApplication)(void*);

/// pointer to value callback function
typedef void(*FlipperRecordCallback)(void*);

/// pointer to state callback function
typedef void(*FlipperRecordStateCallback)(bool);

struct _FuriApp;

typedef struct {
    FlipperRecordCallback cb; ///< value cb
    FlipperRecordStateCallback state_cb; ///< state cb
    _FuriApp* application; ///< pointer to application
} FuriRecordSubscriber;

/// FURI record handler
typedef struct {
    const char* name;
    void* value;
    SemaphoreHandle_t mutex;
    FuriRecordSubscriber subscribers[MAX_RECORD_SUBSCRIBERS];
} FuriRecord;

// store info about active task
struct _FuriApp {
    FlipperApplication application;
    FlipperApplication prev;
    TaskHandle_t handler;
    FuriRecord* records[MAX_TASK_RECORDS]; ///< list of records which task open
};

typedef struct _FuriApp FuriApp;

/*!
Simply starts application.
It call app entrypoint with param passed as argument.
Useful for daemon applications and pop-up.
*/
FuriApp* furiac_start(FlipperApplication app, const char* name, void* param);

/*!
Swtich to other application.
FURI stop current app, call app entrypoint with param passed as
argument and save current application entrypoint to prev field
in current application registry.
Useful for UI or "active" application.
*/
void furiac_switch(FlipperApplication app, char* name, void* param);

/*!
Stop current application
(stop thread and clear application's stack), start application
from prev entry in current application registry, cleanup current
application registry.
*/
void furiac_exit(void* param);

/*!
Stop specified app without returning to prev application.
*/
bool furiac_kill(FuriApp* app);

/*!
Creates named FURI record.
Returns NULL if registry have not enough memory for creating.
*/
FuriRecord* furi_create(char* name);

/*!
Opens existing FURI record by name.
Returns NULL if record does not exist.
\param[in] solo if true another applications handlers set into "muted" state.
When appication has exited or record has closed, all handlers is unmuted.
It may be useful for concurrently acces to resources like framebuffer or beeper.
\param[in] no_mute if true, another applications cannot mute this handler.
*/
FuriRecord* furi_open(char* name, bool solo, bool no_mute);