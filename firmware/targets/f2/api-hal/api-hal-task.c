#include "api-hal-task.h"

bool task_equal(TaskHandle_t a, TaskHandle_t b) {
    if(a == NULL || b == NULL) return false;
    return a == b;
}