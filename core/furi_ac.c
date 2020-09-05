#include "furi.h"
#include "cmsis_os2.h"

// TODO: this file contains printf, that not implemented on uC target

#ifdef FURI_DEBUG
#include <stdio.h>
#endif

#define DEFAULT_STACK_SIZE 512 // Stack size in bytes
#define MAX_TASK_COUNT 8

#ifdef configSUPPORT_STATIC_ALLOCATION
static StaticTask_t task_info_buffer[MAX_TASK_COUNT];
#endif
static uint32_t stack_buffer[MAX_TASK_COUNT][DEFAULT_STACK_SIZE / 4];
static FuriApp task_buffer[MAX_TASK_COUNT];

static size_t current_buffer_idx = 0;

// find task pointer by handle
FuriApp* find_task(osThreadId_t thread) {
    FuriApp* res = NULL;
    for(size_t i = 0; i < MAX_TASK_COUNT; i++) {
        if(task_equal(task_buffer[i].thread, thread)) {
            res = &task_buffer[i];
        }
    }

    return res;
}

FuriApp* furiac_start(FlipperApplication app, const char* name, void* param) {
    #ifdef FURI_DEBUG
        printf("[FURIAC] start %s\n", name);
    #endif

    // TODO check first free item (.thread == NULL) and use it

    if(current_buffer_idx >= MAX_TASK_COUNT) {
        // max task count exceed
        #ifdef FURI_DEBUG
            printf("[FURIAC] max task count exceed\n");
        #endif
        return NULL;
    }

    // create task on static stack memory
    osThreadAttr_t thread_attr = {
      .name = name,
      .priority = osPriorityNormal,
      .stack_mem = stack_buffer[current_buffer_idx],
      .stack_size = DEFAULT_STACK_SIZE,
      .cb_mem = NULL,
      .cb_size = 0
    };
    #ifdef configSUPPORT_STATIC_ALLOCATION
    thread_attr.cb_mem = &task_info_buffer[current_buffer_idx];
    thread_attr.cb_size = sizeof(StaticTask_t);
    #endif
    task_buffer[current_buffer_idx].thread = osThreadNew(app, param, &thread_attr);

    // save task
    task_buffer[current_buffer_idx].application = app;
    task_buffer[current_buffer_idx].prev_name = NULL;
    task_buffer[current_buffer_idx].prev = NULL;
    task_buffer[current_buffer_idx].records_count = 0;
    task_buffer[current_buffer_idx].name = name;

    current_buffer_idx++;

    return &task_buffer[current_buffer_idx - 1];
}

bool furiac_kill(FuriApp* app) {
    #ifdef FURI_DEBUG
        printf("[FURIAC] kill %s\n", app->name);
    #endif

    // check handler
    if(app == NULL || app->thread == NULL) return false;

    // kill task
    osThreadTerminate(app->thread);

    // cleanup its registry
    // TODO realy free memory
    app->thread = NULL;

    return true;
}

void furiac_exit(void* param) {
    // get current task handler
    FuriApp* current_task = find_task(osThreadGetId());

    // run prev
    if(current_task != NULL) {
        #ifdef FURI_DEBUG
            printf("[FURIAC] exit %s\n", current_task->name);
        #endif

        if(current_task->prev != NULL) {
            furiac_start(current_task->prev, current_task->prev_name, param);
        } else {
            #ifdef FURI_DEBUG
                printf("[FURIAC] no prev\n");
            #endif
        }

        // cleanup registry
        // TODO realy free memory
        current_task->thread = NULL;
    }

    // kill itself
     osThreadTerminate(NULL);
}

void furiac_switch(FlipperApplication app, char* name, void* param) {
    // get current task handler
    FuriApp* current_task = find_task(osThreadGetId());

    if(current_task == NULL) {
        #ifdef FURI_DEBUG
            printf("[FURIAC] no current task found\n");
        #endif
    }

    #ifdef FURI_DEBUG
        printf("[FURIAC] switch %s to %s\n", current_task->name, name);
    #endif

    // run next
    FuriApp* next = furiac_start(app, name, param);

    if(next != NULL) {
        // save current application pointer as prev
        next->prev = current_task->application;
        next->prev_name = current_task->name;

        // kill itself
        osThreadTerminate(NULL);
    }
}
