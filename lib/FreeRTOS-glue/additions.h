#pragma once

#include <FreeRTOS.h>
#include <task.h>

void vTaskClearRunTimeStats(TaskHandle_t xHandle);

void vTaskClearTotalRunTimeStats(void);