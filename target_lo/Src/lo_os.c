#include "cmsis_os.h"
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

void osDelay(uint32_t ms) {
    usleep(ms * 1000);
    printf("[DELAY] %d ms\n", ms);
}

typedef struct {
    TaskFunction_t func;
    void * param;
} PthreadTask;

void* pthread_wrapper(void* p) {
    PthreadTask* task = (PthreadTask*)p;
    
    task->func(task->param);

    return NULL;
}

TaskHandle_t xTaskCreateStatic(
    TaskFunction_t pxTaskCode,
    const char * const pcName,
    const uint32_t ulStackDepth,
    void * const pvParameters,
    UBaseType_t uxPriority,
    StackType_t * const puxStackBuffer,
    StaticTask_t * const pxTaskBuffer
) {
    TaskHandle_t thread = malloc(sizeof(TaskHandle_t));
    PthreadTask* task = malloc(sizeof(PthreadTask));

    task->func = pxTaskCode;
    task->param = pvParameters;

    pthread_create(thread, NULL, pthread_wrapper, (void*)task);

    return thread;
}