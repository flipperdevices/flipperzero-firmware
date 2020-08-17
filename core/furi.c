#include "furi.h"
#include "cmsis_os.h"

#define DEFAULT_STACK_SIZE 1024 // Stack size in bytes
#define MAX_TASK_COUNT 8

static StaticTask_t task_buffer[MAX_TASK_COUNT];
static StackType_t stack_buffer[MAX_TASK_COUNT][DEFAULT_STACK_SIZE / 4];

static size_t current_stack_id = 0;

FuriApp furiac_start(FlipperApplication app, const char* name, void* param) {
    FuriApp res;

    if(current_stack_id > MAX_TASK_COUNT) {
        // max task count exceed
        res.handler = NULL;
        return res;
    }

    res.handler = xTaskCreateStatic(
        (TaskFunction_t)app,
        (const char * const)name,
        DEFAULT_STACK_SIZE / 4,
        (void * const) param,
        tskIDLE_PRIORITY + 3,
        stack_buffer[current_stack_id],
        &task_buffer[current_stack_id]
    );

    current_stack_id++;

    return res;
}
