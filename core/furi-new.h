#pragma once
#include "cmsis_os.h"
#include "m-list.h"

#ifdef HAVE_FREERTOS
#include <semphr.h>
#endif

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "assets_icons.h"
#include "furi-deprecated.h"

#define CORE_ERROR UINT32_MAX

typedef osThreadId_t FuriAppId;

LIST_DEF(list_app, FuriAppId);

typedef struct {
    FuriAppId app_id;
    char* name;
    void* payload;
    list_app_t open_by_apps;
} FuriRecordItem;

LIST_DEF(list_furi_record, FuriRecordItem, M_POD_OPLIST);

bool new_furi_init(void);

// creates new record in registry and store pointer into it
bool new_furi_create(const char* name, void* ptr);

// get stored pointer by its name
void* new_furi_open(const char* name, uint32_t timeout);
void* new_furi_open_block(const char* name);

// close record
bool new_furi_close(const char* name);

// delete record from registry
bool new_furi_delete(const char* name);

/* flappity-flapp */
typedef void (*FuriOnExitCallback)(void*);
typedef struct {
    FuriOnExitCallback callback;
    void* context;
} FuriExitCbData;

LIST_DEF(list_furi_exit_callback, FuriExitCbData, M_POD_OPLIST);

typedef void (*FuriAppFn)(void*);

typedef struct {
    FuriAppId app_id;
    char* name;
    list_app_t threads;
    list_furi_exit_callback_t exit_callbacks;
} FuriAppItem;

LIST_DEF(list_furi_app, FuriAppItem, M_POD_OPLIST);

FuriAppItem* find_app_by_thread_id(FuriAppId thread_id);
FuriAppItem* find_app_by_app_id(FuriAppId app_id);

FuriAppId flapp_current_app_id(void);
bool new_flapp_on_exit(FuriOnExitCallback callback, void* context);

FuriAppId new_flapp_app_start(FuriAppFn app, char* name, uint32_t stack_size, void* param);
bool new_flapp_app_stop(FuriAppId app_id);
bool new_flapp_app_exit(void);

FuriAppId new_flapp_thread_start(FuriAppFn app, uint32_t stack_size, void* param);
bool new_flapp_thread_stop(FuriAppId app_id);
bool new_flapp_thread_exit(void);

FuriAppId new_flapp_get_app_id(FuriAppId app_or_thread_id);
uint32_t new_flapp_created_records_count(FuriAppId app_id);
uint32_t new_flapp_opened_records_count(FuriAppId app_id);
uint32_t new_flapp_threads_count(FuriAppId app_id);
uint32_t new_flapp_apps_count();