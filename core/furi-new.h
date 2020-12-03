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

/* 
    struct

    pointer to application
    pointer to name
    pointer to payload
    list of pointers to the application that open the record
*/

LIST_DEF(list_app, osThreadId_t, M_POD_OPLIST);

typedef struct {
    osThreadId_t app_id;
    char* name;
    void* payload;
    list_app_t open_by_apps;
} FuriRecordItem;

LIST_DEF(list_furi_record, FuriRecordItem, M_POD_OPLIST);

bool new_furi_init();

// creates new record in registry and store pointer into it
bool new_furi_create(const char* name, void* ptr);

// get stored pointer by its name
void* new_furi_open(const char* name, uint32_t timeout);
void* new_furi_open_block(const char* name);

// delete record from registry
bool new_furi_delete(const char* name);


/* flappity-flapp */

typedef struct {
    osThreadId_t app_id;
    char* name;
    list_app_t threads;
};